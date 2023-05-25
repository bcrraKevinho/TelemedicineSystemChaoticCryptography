/************************************************************
  Código para el cifrado caótico y transmisión de bioseñales
  por internet.

  Elaborado por: Armando Ceseña Villa
************************************************************/

// Librerías
#include <WiFi.h> // Wi-Fi
#include <SPI.h> // Comunicación módulo SD
#include <SD.h> // SD
#include <LiquidCrystal.h> // LCD

// Factor de división
#define e15 1000000000000000

// Entrada digital para botón
const uint8_t button = 15;

// ***** Configuración módulo de lectura  de memoria micro-SD ***** /////
// Crea un objeto File para manejar el archivo de la SD
File myFile;

// Nombre del archivo con bioseñal a leer en memoria micro-SD
char archivo_Encriptar[] = "/Biosignal.csv";

// Nombre para guardar temporalmente criptograma en micro-SD
char fileName[] = "/Criptograma_Biosignal.csv";

// Tamaño de archivo
long fileSize;

// ******* Variables para envío de datos a servidor ********** /////
// Constantes para configuración WiFi
const char *ssid     = "Red Santamaria";
const char *password = "1035763711";
const char *servername = "192.168.100.104:8012";
byte ip[] = {192,168,100,104};

// Variables para transmisión de datos a servidor
#define MTU_Size  2*1760
byte clientBuf[MTU_Size];
int clientCount, count, chunks;
char symbol[] = {0xE2, 0x96, 0xA0};

char nombre_archivo[] = "Criptograma.csv";

// Crea un objeto "client" para manejar la conexión al servidor
WiFiClient client;

// **************** Variables para encriptado **************** /////
// Clave secreta
const char cadena[]="11223344556677889900AABBCCDDEEFF";

char Llave[4][8];
unsigned long dec_AE, dec_BE, dec_CE, dec_DE, x[4];
double val_AE,val_BE,val_CE,val_DE;

// Señal clara normalizada
double norm[1000];
int LONG = 0;

// Valores para calcular parámetros y condiciones iniciales
double V_PA2, V_CI2, V_PA2_b, V_CI2_b, mod_PA, mod_CI, mod_PA_b, mod_CI_b; 

// Parámetros y condiciones iniciales de mapas caóticos
double a2_L1, b, a_LL1, b1;
double y[1100], x2_L1_1[1100], x2_L1[1100], x1_ENC[1100], y2;
int long_caos2;

// Vector de permutación
int f[1000], per[1000];

// Vector para criptograma
double CRIPTO[1000+5];

// ********************* Configuración LCD ****************** /////
// Conexión LCD-ESP32
LiquidCrystal lcd(12, 14, 32, 33, 25, 26);
//            lcd(Rs, E,  D4, D5, D6, D7)

// Caracter para escribir la letra "ñ"
byte eneMin[8] =
 {
   0b00001110,
   0b00000000,
   0b00010110,
   0b00011001,
   0b00010001,
   0b00010001,
   0b00010001,
   0b00000000
 };

// Caracter para escribir la letra "é"
 byte eAcento[8] =
 {
    0b00000010,
    0b00000100,
    0b00001110,
    0b00010001,
    0b00011111,
    0b00010000,
    0b00001110,
    0b00000000
 };

 // Caracter para escribir la letra "ó"
 byte oAcento[8] = // o
 {
    0b00000010,
    0b00000100,
    0b00001110,
    0b00010001,
    0b00010001,
    0b00010001,
    0b00001110,
    0b00000000
 };

// *********************** Funciones utilizadas ****************** /////

// Función para realizar la operación mod(x,y)
double mod(double X, float Y){
  double z = X - ( floor(X) / Y ) * Y;
  return z;
}

void setup()
{
  Serial.begin(115200);  
  // Inicialización LCD
  lcd.begin(16,2); // LCD 16 columnas 2 lineas
  lcd.createChar(1, eneMin); // Inicializa caracter "ñ"
  lcd.createChar(2, eAcento); // Inicializa caracter "é"
  lcd.createChar(3, oAcento); // Inicializa caracter "ó"

  // Despliega mensaje "SISTEMA DE CIFRADO"
  lcd.clear();
  lcd.setCursor(3,0); 
  lcd.print("SISTEMA DE");
  lcd.setCursor(4,1); 
  lcd.print("CIFRADO");
  delay(4000);
  Serial.println("-----------------mark 0.1----------------------------");
  // Despliega mensaje "Iniciando sistema...."  
  lcd.clear();
  lcd.setCursor(3,0); 
  lcd.print("Iniciando");
  lcd.setCursor(3,1); 
  lcd.print("sistema....");
  delay(2000);
    Serial.println("-----------------mark 0.2----------------------------");
  // Configuración de entradas y salidas
  pinMode(button, INPUT);
  
  // Inicializar memoria SD
  if (!SD.begin()) {
    while(1);
  }
  Serial.println("-----------------mark 0.3----------------------------");
  // Inicializar WiFi  
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(300);
  }
   Serial.println("-----------------mark 0.4----------------------------"); 
  delay(2000);
}

void loop() {
  // Despliega mensaje "Pulse el botón para cifrar"
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Pulse el bot");
  lcd.write(3);  
  lcd.write("n");
  lcd.setCursor(2,1);
  lcd.print("para cifrar");
  Serial.println("-----------------mark 0.5----------------------------");
  //Permite que el ESP32 realice funciones indispensables
  yield();

  // Espera a que se presione el botón
  while(!(digitalRead(button)==true));

  // Despliega mensaje "Leyendo señal médica..."
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Leyendo se");
  lcd.write(1);  
  lcd.write("al");
  lcd.setCursor(5,1);
  lcd.print("m");
  lcd.write(2);  
  lcd.write("dica...");
  delay(1000);
  
  // Abre el archivo CSV que contiene la bioseñal para su lectura
  // y determina min(P)
  char c;
  String bufferString = "";
  double _min=200.0, _max=0.0, var;
  Serial.println("-----------------mark 1----------------------------");
  myFile = SD.open(archivo_Encriptar);
  if (myFile) {
    //Calcula el valor mínimo del ECG
    while (myFile.available()) {
      c = (char)myFile.read();
      if(c != '\n')
      {
        bufferString += c;
      }
      else
      {        
        var = bufferString.toDouble();
        _min = min(_min,var);
        bufferString = "";
        LONG++;
      }
    }
  }
  else {
    return;
  }
  myFile.close();
  Serial.println("-----------------mark 2 ----------------------------");  

  //Primer escalamiento y calcular max(N)   
  myFile = SD.open(archivo_Encriptar);
  if(myFile) {
    while (myFile.available()) {
      c = (char)myFile.read();
      if(c != '\n')
      {
        bufferString += c;
      }
      else
      {
        var = bufferString.toDouble()-(_min-0.01);
        _max = max(_max,var);
        bufferString = "";
      }
    }
  }
  else {
    while(1);
  }
  myFile.close();

  // Normalizar señal entre 0 y 1 (señal transformada)
  double val;
  int i=0;  
    Serial.println("-----------------mark 3 ----------------------------");  
  myFile = SD.open(archivo_Encriptar);
  while (myFile.available())
  {
    c = (char)myFile.read();
    if(c != '\n')
    {
      bufferString += c;
    }
    else
    {
      norm[i] = (bufferString.toDouble()-(_min-0.01))/(_max+0.01);
      bufferString = "";
      i++;
    }
  }
  myFile.close();
  Serial.println("-----------------mark 4 ----------------------------");  
  // Despliega mensaje "Cifrando..."
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Cifrando...");

  // MANEJO DE LLAVE
  // Determinar rangos de llaves dividiendo llave en segmentos de 8
  for(int i=0; i<4; i++)
  {
    for(int j=0; j<8; j++)
    {
      Llave[i][j] = cadena[i*8+j];
    }
    // Convertir hex - dec
    x[i] = strtoul(Llave[i], 0, 16);
  }

  dec_AE = x[0];
  dec_BE = x[1];
  dec_CE = x[2];
  dec_DE = x[3];
  
  // Determinar el valor a sumar en las condiciones iniciales y parámetros
  val_AE = dec_AE / (4294967296 + 1.0);
  val_BE = dec_BE / (4294967296 + 1.0);
  val_CE = dec_CE / (4294967296 + 1.0);
  val_DE = dec_DE / (4294967296 + 1.0);

  // ITERACIÓN DE MAPA SLIM
  
  V_PA2 = mod(val_AE + val_BE,1.0) * 0.001;
  V_CI2 = mod(val_CE + val_DE,1.0);
  
  V_PA2_b = mod(val_AE + val_CE,1.0) * 0.001;
  V_CI2_b = mod(val_BE + val_DE,1.0);
  
  //Parámetros
  a2_L1 = 0.999 + V_PA2;
  b = 7.999 + V_PA2_b;
  
  //Condiciones iniciales
  x2_L1_1[0] = V_CI2;
  y[0] = V_CI2_b;

  // Número de iteraciones de mapa SLIM
  long_caos2 = LONG + 100;
  Serial.println("-----------------mark 5 ----------------------------");    
  for(i=0; i < long_caos2-1; i++)
  {
    // Mapa SLIM
    x2_L1_1[i+1] = sin(b*y[i])*sin(50/x2_L1_1[i]);
    y[i+1] = a2_L1*(1-2*x2_L1_1[i+1]*x2_L1_1[i+1])*sin(50/y[i]);

    // Optimización de secuencia X
    x2_L1[i] = mod(x2_L1_1[i]*1000,1.0);      
  }

  // Calcular el valor de Z
  double SUMA;
  for(i=0;i<LONG;i++)
  {
    SUMA = SUMA + ( (norm[i]+1) * x2_L1[long_caos2-1-i]) + x2_L1[long_caos2-1-i];
  }
  // Solo valores entre 0 - 1
  SUMA = floor(mod(SUMA,1.0)*e15)/e15;

  // ITERACIÓN DE MAPA SENO-HÉNON REALIMENTADO PARA SECUENCIAS DE PERMITACIÓN-DIFUSIÓN
  
  mod_PA = mod(val_AE + val_BE + SUMA,1.0) * 0.001;
  mod_CI = mod(val_CE + val_DE + SUMA,1.0);
  
  mod_PA_b = mod(val_AE + val_CE + SUMA,1.0) * 0.001;
  mod_CI_b = mod(val_BE + val_DE + SUMA,1.0);
  
  //Parámetros
  a_LL1 = 9.999 + mod_PA;
  b1 = 8.999 + mod_PA_b;
  
  //Condiciones iniciales
  x1_ENC[0] = mod_CI;
  y2 = mod_CI_b;
  Serial.println("-----------------mark 6----------------------------");  
  for(i=0; i < long_caos2-1; i++)
  {
    // Mapa Seno-Hénon realimentadocon secuencias de mapa SLIM
    x1_ENC[i+1] = mod(1-a_LL1*(sin(x1_ENC[i])*sin(x1_ENC[i]))+y2+ x2_L1_1[i],1.0);
    y2 = mod(b1*x1_ENC[i] + y[i] ,1.0);
  }

  // SECUENCIA PARA PERMUTACION
  
  for(i=0;i<LONG;i++)
  {
    per[i]=round((x1_ENC[(long_caos2)-LONG + i]*(LONG-1)));
  }

  // Optimización de secuencia de permutación
  // Busca posiciones de repetidos y los pone en un vector
  f[0] = per[0];
  int s=0,j, k, vect_r2D[500];
  byte en; 
  for(k=1;k<LONG;k++)
  {
    en=1;
    f[k]=per[k];
    for(j=0;j<(k);j++)
    {
      if(f[k] == f[j] && en == 1)
      {
        vect_r2D[s]=k;
        s=s+1;
        en=0;
      }
    }
  }

  // Buscar numeros que no esten en vector de permutación
  int preg_num=0, no=0, num_no_est2D[500];
  byte saltar=0;
  for(int v=0; v<LONG; v++)
  {
    // Pregunta si esta en vector de permutación
    for(int vvv=0; vvv<LONG; vvv++)
    {
      if(preg_num == per[vvv])
      {
        saltar=1; // Si esta entra.
      }      
    }
    // Si no esta en repetidos y en vector de permutación, entra
    if(saltar == 0)
    {
      num_no_est2D[no]=preg_num; // Guarda número.
      no=no+1;
    }
    saltar=0;   
    preg_num=preg_num+1;
  }

  Serial.println("-----------------mark 7 ----------------------------");  
  // Se actualiza vector de permutación (tiene todos los espacios)
  int tam_vect=s;
  byte en_dos=round(tam_vect/2);  
  if(tam_vect%2 != 0)
  {
    en_dos = en_dos +1;
  }
  
  byte cambio=1;
  int S=0;
  for(s=0; s<tam_vect; s++)
  {
    if(cambio == 1)
    {
     per[vect_r2D[s]]=num_no_est2D[S];
     cambio=0;
    }
    else
    {
     per[vect_r2D[s]]=num_no_est2D[S+en_dos];
     S=S+1;
     cambio=1; 
    }       
  }

  // SECUENCIA PARA DIFUSION  
  for(i=0; i<LONG; i++)
  {
    y[i] = mod((x1_ENC[long_caos2-LONG + i]*1000) + SUMA,1.0);
    y[i] = floor(y[i]*e15)/e15;
  }

  // PROCESO DE PERMUTACION y DIFUSION (CIFRADO)  
  for(i=0; i<LONG; i++)
  {
    CRIPTO[i] = mod(norm[per[i]] +  y[i] +  y[LONG-i-1],1.0);
  }

  // Se añaden los últimos 5 datos al final del criptograma
  CRIPTO[LONG] = SUMA;
  if(_min < 0)
  {
    CRIPTO[LONG + 1] = 1.000000000000000; //Si es -, es 1
  }
  else
  {
    CRIPTO[LONG + 1] = 0.000000000000000; //Si es +, es 0
  }
  CRIPTO[LONG + 2] = abs(_min)/100000.0;
  if(_max < 0)
  {
    CRIPTO[LONG + 3] = 1.000000000000000;; //Si es -, es 1
  }
  else
  {
    CRIPTO[LONG + 3] = 0.000000000000000; //Si es +, es 0
  }
  CRIPTO[LONG + 4] = abs(_max)/100000.0;

  // ********************* Transmisión de criptograma **************** /////
  Serial.println("-----------------mark 8 ----------------------------");  
  // Despliega mensaje "Enviando a servidor..."
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Enviando a");
  lcd.setCursor(2,1);
  lcd.print("servidor...");
  delay(1000);

  // Guarda cripgrama en un archivo en memoria micro-SD  
  myFile = SD.open(fileName, FILE_WRITE);
  for(i=0; i<LONG+5; i++)
  {     
    myFile.println(CRIPTO[i],15);
  }
  myFile.close();

  // Calcula tamaño de archivo
  myFile = SD.open(fileName, FILE_READ);
  fileSize = myFile.size();
  chunks = fileSize/(16*MTU_Size);
  count = 0;

  // Reaiza transmisión de archivo a servidor local
  if(client.connect(ip, 8012))
  {
      Serial.println("-----------------mark 8.1----------------------------");
    //HTTP POST request:

    String head = "----84989444e2484915a216e1718e0f93f0\r\nContent-Disposition: form-data; name=\"FileGDF\"; filename=\"" + String(nombre_archivo) + "\"\r\nContent-Type: application/octet-stream\r\n\r\n";
    String tail = "\r\n----84989444e2484915a216e1718e0f93f0--\r\n";

    client.println("POST /tesis/upload.php?up=1&ACM=1 HTTP/1.1");
    client.print("Host: ");
    client.println(servername);
    client.println("Connection: close");
    client.println("Content-Type: multipart/form-data; boundary=--84989444e2484915a216e1718e0f93f0");
    client.print("Content-Length: ");client.println(myFile.size() + head.length() + tail.length()); 
    client.println();
    Serial.println();
    Serial.println(myFile.size() + head.length() + tail.length());
    Serial.println();
    client.println("----84989444e2484915a216e1718e0f93f0");
    client.println("Content-Disposition: form-data; name=\"FileGDF\"; filename=\"" + String(nombre_archivo) + "\"");
    client.println("Content-Type: application/octet-stream");
    client.println();

    client.setNoDelay(1); 
    while(myFile.available())
    {
      Serial.print(count);
      Serial.print(": ");
      Serial.println(myFile.available());
      if(myFile.available() >= MTU_Size)
      {
        clientCount = MTU_Size;
      }
      else
      {
        clientCount = myFile.available();
      }
      
      myFile.read(&clientBuf[0],clientCount);
      
      client.write((const uint8_t *)&clientBuf[0], clientCount);
      count++;
      if(count == chunks)
      {
        count = 0;
      }
    }
    
    client.println();
    client.println("----84989444e2484915a216e1718e0f93f0--"); //form end
    client.println();
    
    myFile.close();
    // Elimina criptograma de la memoria micro-SD
    SD.remove(fileName);
  }
  else
  {
  Serial.println("fail");  
  }
  Serial.println("-----------------mark 9 ----------------------------");  
  // Respuesta del servidor
  while(client.available())
  {
  }  
  client.stop();

  // Despliega mensaje "Proceso terminado!"
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("Proceso");
  lcd.setCursor(3,1);
  lcd.print("terminado!");
  
  delay(5000);
}
<?php
// Read the CSV file
$data = array();
$file = fopen("cifrado.csv","r");
while(! feof($file))
{
  $data[] = fgetcsv($file);
}
fclose($file);

$data2 = array();

if(isset($_POST['BtnDescifrar']))
{
    if(isset($_POST['InputClave']))
    {
        if($_POST['InputClave'] == "11223344556677889900AABBCCDDEEFF")
        {
            $data2 = array();
            $file2 = fopen("descifrado.csv","r");
            while(! feof($file2))
            {
                $data2[] = fgetcsv($file2);
            }
            fclose($file2);
        }
    }
}
?>

<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ECG Graph</title>
  <script src="https://cdn.plot.ly/plotly-latest.min.js"></script>
  <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/css/bootstrap.min.css">
  <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/5.15.3/css/all.min.css">
</head>
<body>
  <header>
    <nav class="navbar navbar-expand-md navbar-dark bg-dark">
      <a href="#" class="navbar-brand">Sistema de Telemedicina Seguro</a>
      <button class="navbar-toggler" type="button" data-toggle="collapse" data-target="#navbarNav">
        <span class="navbar-toggler-icon"></span>
      </button>
      <div class="collapse navbar-collapse" id="navbarNav">
        <ul class="navbar-nav ml-auto">
          <li class="nav-item active"><a href="#" class="nav-link">Home</a></li>
          <li class="nav-item"><a href="servicios.html" class="nav-link">Servicios</a></li>
          <li class="nav-item"><a href="#" class="nav-link">Nosotros</a></li>
          <li class="nav-item"><a href="#" class="nav-link">Contáctanos</a></li>
        </ul>
      </div>
    </nav>
  </header>
  <main class="container my-5">
    <div class="row">
      <div class="col-5">
        <div class="card">
          <div class="card-header font-weight-bold text-center">Electrocardiograma Cifrado</div>
          <div class="card-body">
            <div id="ecg-graph"></div>
          </div>
        </div>
      </div>
      <div class="col-2">
        
      
    <form method="post">
        <div class="form-group">
            <label for="InputClave">Clave de descrifrado</label>
            <input type="password"  class="form-control" name="InputClave" id="InputClave" maxlength="32" aria-describedby="claveHelp" placeholder="Ingresa la clave">
            <!-- <small id="emailHelp" class="form-text text-muted">Recuerda no compartir esta clave con nadie.</small> -->
        </div>
        <button type="submit" name="BtnDescifrar" id="btnDescifrar" class="btn btn-primary">Descifrar</button>
    </form>

      </div>
      <div class="col-5">
        <div class="card">
          <div class="card-header font-weight-bold text-center">Electrocardiograma Descifrado</div>
          <div class="card-body">
            <div id="ecg-graph2"></div>
          </div>
        </div>
      </div>
    </div>
  </main>
  <footer class="bg-dark text-white py-3">
    <div class="container">
      <div class="row">
    <div class="col-md-4">
      <h3>Contáctanos</h3>
      <p>Empresa</p>
      <p>Dirección</p>
      <p>Ciudad, Estado CP</p>
      <p>Teléfono: XXX-XXX-XXXX</p>
      <p>Email: correo@gmail.com</p>
      <ul class="list-inline">
        <li class="list-inline-item"><a href="#" class="text-white"><i class="fab fa-facebook fa-2x"></i></a></li>
        <li class="list-inline-item"><a href="#" class="text-white"><i class="fab fa-twitter fa-2x"></i></a></li>
        <li class="list-inline-item"><a href="#" class="text-white"><i class="fab fa-instagram fa-2x"></i></a></li>
        <li class="list-inline-item"><a href="#" class="text-white"><i class="fab fa-linkedin fa-2x"></i></a></li>
      </ul>
    </div>
    <div class="col-md-4">
      <h3>Enlaces útiles</h3>
      <ul class="list-unstyled">
        <li><a href="index.html" class="text-white">Home</a></li>
        <li><a href="servicios.html" class="text-white">Servicios</a></li>
        <li><a href="#" class="text-white">Nosotros</a></li>
        <li><a href="#" class="text-white">Contáctanos</a></li>
      </ul>
    </div>
    <div class="col-md-4">
      <h3>Newsletter</h3>
      <form>
        <div class="form-group">
          <input type="email" class="form-control" placeholder="Email Address">
        </div>
        <button type="submit" class="btn btn-primary">Suscríbete</button>
      </form>
    </div>
  </div>
</div>
</footer>
  <script>
    var arrayx = [...Array(1000).keys()];
    arrayx.forEach(function (element, index, array) {
      array[index] = element * 0.01 + 0.01;
    });
    

    var data = {
      x: arrayx,
      y: [
        <?php 
    foreach ($data as $row) {
        if(is_array($row))
        {
            echo $row[0].", ";
        }
    }
    ?>
      ],
      type: 'scatter',
      mode: 'lines',
      line: {
        color: 'red',
        width: 1
      }
    };
    
    var data2 = {
      x: arrayx,
      y: [
        <?php 
        foreach ($data2 as $row) 
        {
            if(is_array($row))
            {
                echo $row[0].", ";
            }
        }
        ?>
      ],
      type: 'scatter',
      mode: 'lines',
      line: {
        color: 'red',
        width: 2
      }
    };

    var layout = {
      xaxis: {
        title: 'Time (s)'
      },
      yaxis: {
        title: 'Voltage (mV)'
      }
    };

    Plotly.newPlot('ecg-graph', [data], layout);
    Plotly.newPlot('ecg-graph2', [data2], layout);
  </script>
  <script src="https://code.jquery.com/jquery-3.2.1.slim.min.js"></script>
  <script src="https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.12.9/umd/popper.min.js"></script>
  <script src="https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/js/bootstrap.min.js"></script>
</body>
</html>

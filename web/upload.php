<?php
//echo $_FILES["FileGDF"]["size"];
//echo $_FILES["FileGDF"]["tmp_name"];
//echo $_FILES["FileGDF"]["error"];
echo "HOLA";
$target_dir = "uploads/";
$filename = basename(@$_FILES["FileGDF"]["name"]);
$target_file = $target_dir . $filename;
$FileType = pathinfo($target_file,PATHINFO_EXTENSION);
if(isset($_GET["up"])&&(isset($_GET["ACM"]))) {
  if (move_uploaded_file(@$_FILES["FileGDF"]["tmp_name"], $target_file)) 
  {
    echo "The file ". basename($filename). " has been uploaded.";
  } 
  else 
  {
    //echo "Sorra";
    echo "Sorry, there was an error uploading your file. Error #";
    echo $_FILES["FileGDF"]["error"];
  }
}
echo ". TempName: ";
echo $_FILES["FileGDF"]["tmp_name"];
//echo $target_file;
// para raspberry pi chown www-data:www-data /var/www/html -R && chmod 0775 /var/www/html/AutoPost1.0/Dev/posts/post_images
?>
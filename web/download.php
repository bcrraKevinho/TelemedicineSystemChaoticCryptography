<?php
$target_dir = "uploads/";
$filename = $_GET["name"];
$target_file = $target_dir . $filename;


if (file_exists($target_file)) {
    header('Content-Description: File Transfer');
    header('Content-Type: application/octet-stream');
    header('Content-Disposition: attachment; filename="'.basename($target_file).'"');
    header('Content-Length: ' . filesize($target_file));
    echo("\t");
    readfile($target_file);
}
?>
<?php

$requestedDirectory = $_SERVER['REQUEST_URI'];
$lastSlashPosition = strrpos($requestedDirectory, '/');
if ($lastSlashPosition !== false) {
    $requestedDirectory = substr($requestedDirectory, 0, $lastSlashPosition);
}

$directoryPath = $_SERVER['DOCUMENT_ROOT'] . $requestedDirectory;

if (is_dir($directoryPath)) {
    $directory = opendir($directoryPath);

    echo '<h1>Conteúdo do Diretório: ' . $requestedDirectory . '</h1>';
    echo '<ul>';
    while (($file = readdir($directory)) !== false) {
        if ($file != '.' && $file != '..') {
            $filePath = $requestedDirectory . '/' . $file;
            echo '<li><a href="' . $filePath . '">' . $file . '</a></li>';
        }
    }
    echo '</ul>';

    closedir($directory);
} else {
    echo '<h1>Diretório não encontrado: ' . $requestedDirectory . '</h1>';
}
?>

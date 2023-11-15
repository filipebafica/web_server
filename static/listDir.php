<!DOCTYPE html>
  <html lang="en">
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Explorador de Diretórios</title>
    <style>
      body {
        font-family: Arial, sans-serif;
        margin: 20px;
        background-color: #050505;
      }
      h1 {
        color: #ff0534;
      }
      ul {
        list-style-type: none;
        padding: 0;
      }
      li {
        margin-bottom: 5px;
      }
      a {
        text-decoration: none;
        color: #01c4e7;
      }
      a:hover {
        text-decoration: underline;
      }
    </style>
  </head>
  <body>
  <?php

  $requestedDirectory = $_SERVER['REQUEST_URI'];
  $lastSlashPosition = strrpos($requestedDirectory, '/', 1);
  if ($lastSlashPosition !== false) {
    $requestedDirectory = substr($requestedDirectory, 1, $lastSlashPosition);
  }

  $directoryPath = $_SERVER['DOCUMENT_ROOT'] . '/' . $requestedDirectory;

  if (is_dir($directoryPath)) {
    $directory = opendir($directoryPath);

    echo '<h1>Conteúdo do Diretório: ' . $requestedDirectory . '</h1>';
    echo '<ul>';
    while (($file = readdir($directory)) !== false) {
      if ($file != '.' && $file != '..') {
        $filePath = $directoryPath . '/' . $file;
        if (is_dir($filePath)) {
          $file = $file . "/";
        }
        $uriPath = $requestedDirectory . $file;
        echo '<li><a href="' . urlencode($uriPath) . '">' . $file . '</a></li>';
      }
    }
    echo '</ul>';

    closedir($directory);
  } else {
    echo '<h1>Diretório não encontrado: ' . $requestedDirectory . '</h1>';
  }
  ?>
  </body>
</html>

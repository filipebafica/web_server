<?php
// Pega a parte da URL após o nome do arquivo (excluindo a barra inicial)
$requestedDirectory = ltrim($_SERVER['REQUEST_URI'], '/');
$directoryPath = __DIR__ . '/' . $requestedDirectory;

// Verifica se o caminho do diretório existe e é um diretório válido
if (is_dir($directoryPath)) {
    // Abre o diretório
    $directory = opendir($directoryPath);

    // Exibe uma lista de links para os arquivos no diretório
    echo '<h1>Conteúdo do Diretório: ' . $requestedDirectory . '</h1>';
    echo '<ul>';
    while (($file = readdir($directory)) !== false) {
        if ($file != '.' && $file != '..') {
            $filePath = $requestedDirectory . '/' . $file;
            echo '<li><a href="' . $filePath . '">' . $file . '</a></li>';
        }
    }
    echo '</ul>';

    // Fecha o diretório
    closedir($directory);
} else {
    // Caso o diretório não exista, exibe uma mensagem de erro
    echo '<h1>Diretório não encontrado: ' . $requestedDirectory . '</h1>';
}
?>

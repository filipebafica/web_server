<?php

print_r($_FILES);
print_r($_SERVER);

if (isset($_FILES["file"]) == false) {
    echo "Nenhum arquivo foi enviado";
    return;
}

if ($_FILES["file"]["error"] === UPLOAD_ERR_OK) {
    $target_file = basename($_FILES["file"]["name"]);

    if (move_uploaded_file($_FILES["file"]["tmp_name"], $target_file)) {
        echo "Arquivo enviado com sucesso.";
    } else {
        echo "Ocorreu um erro ao enviar o arquivo.";
    }
} else {
    echo "Erro no upload do arquivo: ";
    echo $_FILES["file"]["error"];
}
?>

<?php

print_r($_FILES);
print_r($_SERVER);

if (isset($_FILES["file"]) && $_FILES["file"]["error"] === UPLOAD_ERR_OK) {
    $target_file = basename($_FILES["file"]["name"]);

    $imageFileType = strtolower(pathinfo($target_file, PATHINFO_EXTENSION));
    if($imageFileType === "txt") {
        if (move_uploaded_file($_FILES["file"]["tmp_name"], $target_file)) {
            echo "Arquivo enviado com sucesso.";
        } else {
            echo "Ocorreu um erro ao enviar o arquivo.";
        }
    } else {
        echo "Apenas arquivos TXT são permitidos.";
    }
} else {
    echo "Nenhum arquivo foi enviado ou ocorreu um erro.";
}
?>

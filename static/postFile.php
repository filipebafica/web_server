<?php
$target_dir = "."; // Diretório onde o arquivo será armazenado

if(isset($_FILES["fileToUpload"]) && $_FILES["fileToUpload"]["error"] === UPLOAD_ERR_OK) {
    // Verifique se o envio ocorreu sem erros

    $target_file = $target_dir . basename($_FILES["fileToUpload"]["name"]);

    // Verifique o tipo de arquivo (opcional)
    $imageFileType = strtolower(pathinfo($target_file, PATHINFO_EXTENSION));
    if($imageFileType === "txt") {
        // O arquivo é do tipo TXT

        // Movendo o arquivo para o diretório de destino
        if(move_uploaded_file($_FILES["fileToUpload"]["tmp_name"], $target_file)) {
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

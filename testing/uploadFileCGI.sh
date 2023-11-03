REDIRECT_STATUS=true
REQUEST_METHOD=POST
GATEWAY_INTERFACE=CGI/1.1
CONTENT_LENGTH=230
CONTENT_TYPE='multipart/form-data; boundary=---------------------------126829857566315249935482909'
REQUEST_URI=/                           # URI can be any one
SCRIPT_FILENAME=./static/postFile.php   # Script is fixed
export REDIRECT_STATUS
export SCRIPT_FILENAME
export REQUEST_METHOD
export GATEWAY_INTERFACE
export CONTENT_LENGTH
export CONTENT_TYPE
echo -e "
-----------------------------126829857566315249935482909
Content-Disposition: form-data; name="fileToUpload"; filename="helloToCGI.txt"
Content-Type: text/plain

Hello CGI
-----------------------------126829857566315249935482909--\r\n" | php-cgi
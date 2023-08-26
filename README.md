# web_server
A web_server in C++

````
Start
|
|--> Create server socket
|    |
|    |--> Setup server address
|    |
|    |--> Bind server socket to address
|
|--> Prepare server socket to accept calls
|
|--> Loop (while true)
|    |
|    |--> Update list of file descriptors to poll
|    |
|    |--> Poll for events on file descriptors
|    |    |
|    |    |--> If pollEvents() == -1
|    |    |    |
|    |    |    |--> Print error message
|    |    |    |--> Continue loop
|    |    |
|    |    |--> Check if server socket has events (new client connection)
|    |    |    |
|    |    |    |--> If yes
|    |    |    |    |
|    |    |    |    |--> Accept new client connection
|    |    |    |
|    |    |    |--> Handle requests from existing clients
|    |    |    |    |
|    |    |    |    |--> Loop over client sockets
|    |    |    |    |    |
|    |    |    |    |    |--> If client socket has POLLIN event
|    |    |    |    |    |    |
|    |    |    |    |    |    |--> Read request from client
|    |    |    |    |    |    |--> Process request
|    |    |    |    |    |    |--> Send response to client
|    |    |    |    |    |    |--> Close client socket
|    |    |    |    |    |    |--> Remove client socket from list
|
|--> Terminate program


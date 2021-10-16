/**
 * Build socket functionality depending on whether the need is to create a socket for client or server.
 * 
 * @param char* flag Type of flags passed by console, determines if it will be client or server.
 * @param int Port where we will launch the client or server socket.
 * @param char* destination ip where we start the client or server socket.
 * @return void
*/
extern const void builder(char*, int, char*);

/**
 * We create the client socket with the necessary characteristics
 * 
 * @param char* destination ip where we start the client socket.
 * @param char* flag Type of flags passed by console
 * @param int Port where we will launch the client socket.
 * @return void
*/
extern const void builderClient(char *, char *, int);

/**
 * We create the server socket with the necessary characteristics
 * 
 * @param char* destination ip where we start the server socket.
 * @param int Port where we will launch the client socket.
 * @return void
*/
extern const void builderServer(char *, int);
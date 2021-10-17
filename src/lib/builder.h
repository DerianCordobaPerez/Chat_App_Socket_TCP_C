#include "libs.h"

/**
 * Build socket functionality depending on whether the need is to create a socket for client or server.
 * 
 * @param char** Data obtained by console.
 * @return void
*/
extern const void builder(char**);

/**
 * We create the client socket with the necessary characteristics
 * 
 * @param char* destination ip where we start the client socket.
 * @param int Port where we will launch the client socket.
 * @return void
*/
extern const void builderClient(char *, int);

/**
 * We create the server socket with the necessary characteristics
 * 
 * @param int Port where we will launch the client socket.
 * @param int Number of clients to connect.
 * @return void
*/
extern const void builderServer(int, int);
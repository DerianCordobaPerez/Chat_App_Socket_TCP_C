/**
 * Sends the given message from the server to all clients.
 * 
 * @param ClientNode* client Client with the session started.
 * @param char* buffer temporary buffer.
 * @return void
*/
extern const void sendMessageAllClients(ClientNode*, char*);

/**
 * We handle the client when he is logging into the server.
 * 
 * @param void* client Client to handle
 * @param void
*/
extern const void handlerClient(void *);

/**
 * Terminate the execution of the application according to the flag.
 * 
 * @param int signal Signal sent.
 * @return void
*/
extern void exitProgramServer(int);
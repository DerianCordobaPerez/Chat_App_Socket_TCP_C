/**
 * Sends the given message from the server to all clients.
 * 
 * @param int id ID Client with the session started.
 * @param char* buffer temporary buffer.
 * @return void
*/
extern const void sendMessageAllClients(int, char*);

/**
 * We handle the client when he is logging into the server.
 * 
 * @param void* client Client to handle
 * @param void*
*/
extern void *handlerClient(void *);

/**
 * Add the new past customer as a parameter.
 * 
 * @param Client* client Current client add.
 * @param void
*/
extern const void addClient(Client*);

/**
 * Delete the current client using the id.
 * 
 * @param int id Current client ID.
*/
extern const void deleteClient(int);
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

/**
 * Gets the information of all connected clients.
 * 
 * @param void
 * @return void
*/
extern const void getInformationClients(void);

/**
 * Send a private message to the client with the name passed as a parameter.
 * 
 * @param char* name Client name.
 * @param char* message Message to send.
 * @return void
*/
extern const void sendPrivateMessageClient(char*, char*);

/**
 * It obtains the information of the clients connected with the client 
 * that has the id passed as a parameter.
 * 
 * @param int id Client ID.
 * @return void
*/
extern const void getConnectionsClients(int);
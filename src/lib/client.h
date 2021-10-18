/**
 * We handle the sending of messages.
 * 
 * @param int socket used
 * @return void
*/
extern const void sendMessage();

/**
 * We will get the messages sent.
 * 
 * @param int socket used
 * @return void
*/
extern const void getMessage();

/**
 * Sets the name of the file.
 * 
 * @param void
 * @return void
*/
extern const void setNameFile(void);

/**
 * Sets the name of the client.
 * 
 * @param void
 * @return void
*/
extern const void setName(void);

/**
 * Write the message passed as a parameter to the file.
 * 
 * @param char* message Message that we will write in the file.
 * @return void
*/
extern const void saveFile(char*);

/**
 * Execute a send command to the server.
 * 
 * @param char* buffer Buffer to write.
 * @param char* command Command to execute.
 * @return void
*/
extern const executeCommand(char*, char*);

/**
 * Terminate the execution of the application according to the flag.
 * 
 * @param int signal Signal sent.
 * @return void
*/
extern const void exitProgram(int);
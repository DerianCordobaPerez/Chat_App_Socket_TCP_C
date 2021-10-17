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
 * Terminate the execution of the application according to the flag.
 * 
 * @param int signal Signal sent.
 * @return void
*/
extern const void exitProgram(int);
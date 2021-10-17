// Client structure which we will use as a doubly linked list.
typedef struct {
    struct sockaddr_in address;
	char name[50];
	int sockfd, id;
} Client;

/**
 * Send a separate error message terminate program execution.
 * 
 * @param char* message Error message.
 * @return void
*/
extern const void error(char*);

/**
 * We will validate if the number of parameters passed is correct.
 * 
 * @param int argc Number of arguments.
 * @return void
*/
extern const void validateParams(int);

/**
 * Returns an array assigned with the generic pointer.
 * 
 * @param int quantity Amount of memory to allocate.
 * @return void* Returns an array.
*/
extern void* memoryAllocation(int);

/**
 * The console prints the desired format for the client when a message is sent.
 * 
 * @return void
*/
extern const void printWithFormat();

/**
 * Sent messages are given a specific format.
 * 
 * @param char* message Message to be formatted.
 * @param int length Message length.
 * @return void
*/
extern const void stringFormat(char *, int);

/**
 * 
 * 
 * @param char* command
 * @param void
*/
extern const void executeCommand(char*);
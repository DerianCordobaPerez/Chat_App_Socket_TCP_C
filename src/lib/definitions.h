// Client structure which we will use as a doubly linked list.
typedef struct {
    struct sockaddr_in address;
	char name[50];
	int socket, id;
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
 * @param char* name Name client
 * @param int socket Socket client
 * @return void
*/
extern const void printWithFormat(char*, int);

/**
 * Sent messages are given a specific format.
 * 
 * @param char* message Message to be formatted.
 * @param int length Message length.
 * @return void
*/
extern const void stringFormat(char *, int);

/**
 * Returns the current system time.
 * 
 * @return char* Current time.
*/
extern char* getCurrentTime();

/**
 * Delete space in left side.
 * 
 * @param char* string 
 * @return char*
*/
extern char* ltrim(char*);

/**
 * 
 * 
 * @param char* string 
 * @return char*
*/
extern char* rtrim(char*);

/**
 * 
 * 
 * @param char* string
 * @return char*
*/
extern char* trim(char*);
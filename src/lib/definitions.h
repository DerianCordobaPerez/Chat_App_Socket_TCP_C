// Client structure which we will use as a doubly linked list.
typedef struct Client {
    int socket;
    struct Client *previuos, *next;
    char *ip, *name;
} ClientNode;

/**
 * Create a new client with the corresponding socket and ip.
 * 
 * @param int socket Socket used.
 * @param char* ip Ip assign.
 * @return ClientNode New client node created.
 * 
*/
extern ClientNode *newClient(int, char*);

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
extern const void* memoryAllocation(int);

/**
 * Get the ip from the socket passed as a parameter.
 * 
 * @param char* Adapter from which we will obtain the ip.
 * @return char* Returns the ip obtained from the socket.
*/
extern const char* getIp(char *);

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
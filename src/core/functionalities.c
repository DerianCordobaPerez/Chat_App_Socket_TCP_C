#include "../lib/main.h"

extern const void error(char *message) {
    puts(message);
    exit(EXIT_FAILURE);
}

extern const void validateParams(int argc) {
    if(argc < 3 && argc > 5)
        error("The number of parameters passed does not match the required ones\nExpected format for server ./chat <flag> <port>\nExpected format for client ./chat <flag> <port> <destination>");
}

extern void *memoryAllocation(int quantity) {
    void *pointer;

    if((pointer = (void*)malloc(quantity * sizeof(void))) == NULL)
        error("Failed to allocate memory inside the pointer");

    return pointer;
}

extern const void printWithFormat() {
    printf("\r%s", "> ");
    fflush(stdout);
}

extern const void stringFormat(char *message, int length) {
    for(int i = 0; i < length; ++i) {
        if(message[i] == '\n') {
            message[i] = '\0';
            break;
        }
    }
}

extern const void executeCommand(char *command) {
    
    
}
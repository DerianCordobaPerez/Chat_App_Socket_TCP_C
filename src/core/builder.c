#include "builderServer.c"
#include "builderClient.c"

extern const void builder(char **args) {

    // We validate that the flag passed as a parameter is not correct.
    if(strcmp(args[1], "-c") != 0 && strcmp(args[1], "-l") != 0)
        error("The flag passed as a parameter is not correct.");

    // We verify through the flag if it is different from the client's flag.
    if(strcmp(args[1], "-l") == 0) {
        // We create the builder for the server socket.
        builderServer(atoi(args[2]), args[4] ? atoi(args[4]) : 5);
        return;
    }

    // We create the builder for the client socket.
    builderClient(args[3], atoi(args[2]));
}
#include "builderServer.c"
#include "builderClient.c"

extern const void builder(char* flag, int port, char* destination) {

    // We validate that the flag passed as a parameter is not correct.
    if(strcmp(flag, "-c") != 0 && strcmp(flag, "-l") != 0)
        error("The flag passed as a parameter is not correct.");

    // We verify through the flag if it is different from the client's flag.
    if(strcmp(flag, "-l") == 0) {
        // We create the builder for the server socket.
        builderServer(flag, port);
        return;
    }

    // We create the builder for the client socket.
    builderClient(destination, flag, port);
}
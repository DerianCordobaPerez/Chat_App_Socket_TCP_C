#include "./core/core.c"

int main(int argc, char *argv[]) {
    
    printf("%i\n", argc);

    // Validate that the parameters passed by the console are the necessary ones.
    validateParams(argc);

    // We create the socket regardless of whether it is a client or a server.
    builder(argv[1], atoi(argv[2]), argv[3] ? argv[3] : "");

    return EXIT_SUCCESS;
}
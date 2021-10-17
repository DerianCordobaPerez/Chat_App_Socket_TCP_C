#include "./core/core.c"

int main(int argc, char *argv[]) {

    // Validate that the parameters passed by the console are the necessary ones.
    validateParams(argc);

    // We create the socket regardless of whether it is a client or a server.
    builder(argv);

    return EXIT_SUCCESS;
}
#include "../lib/builder.h"
#include "../lib/client.h"

volatile static int flagExit = 0;

extern void exitProgramClient(int signal) {
    flagExit = 1;
}

extern const void getMessage(int socket) {
    char *message = (char*)memoryAllocation(100);

    for(;;) {
        long int received;
        if((received = recv(socket, message, 100, 0)) > 0) {
            printf("\r%s\n", message);
            printWithFormat();
        } else if(received == 0) break;
    }
    free(message);
}

extern const void sendMessage(int socket) {
    char *message = (char*)memoryAllocation(100);
    for(;;) {
        printWithFormat();
        while(fgets(message, 100, stdin) != NULL) {
            stringFormat(message, 100);
            if(!strlen(message)) {
                printWithFormat();
                continue;
            }
            break;
        }

        send(socket, message, 100, 0);
        
        if(!strcmp(message, "exit")) break;
    }
    free(message);
    exitProgramClient(2);
}

extern const void builderClient(char *destination, char* flag, int port) {

    // We create the general signal for the use of the exit function.
    signal(SIGINT, exitProgramClient);

    // Memory allocation for username
    char* name = (char*)memoryAllocation(50);
    memset(name, '\0', sizeof*name);

    // Naming
    printf("Enter name: ");
    if (fgets(name, 100, stdin) != NULL)
        stringFormat(name, 100);

    // Create socket
    int client = socket(AF_INET , SOCK_STREAM , 0);
    if (client == -1)
        error("Fail to create a socket.");

    // Socket information
    struct sockaddr_in serverInfo, clientInfo;
    int serverLength = sizeof(serverInfo), clientLength = sizeof(clientInfo);
    memset(&serverInfo, 0, serverLength);
    memset(&clientInfo, 0, clientLength);
    serverInfo.sin_family = PF_INET;
    serverInfo.sin_addr.s_addr = inet_addr(destination);
    serverInfo.sin_port = htons(port);

    // Connect to Server
    int err = connect(client, (struct sockaddr *)&serverInfo, serverLength);
    if (err == -1)
        error("Connection to Server error!\n");
    
    // Names
    getsockname(client, (struct sockaddr*) &clientInfo, (socklen_t*) &clientLength);
    getpeername(client, (struct sockaddr*) &serverInfo, (socklen_t*) &serverLength);
    printf("Connect to Server: %s:%d\n", inet_ntoa(serverInfo.sin_addr), ntohs(serverInfo.sin_port));
    printf("You are: %s:%d\n", inet_ntoa(clientInfo.sin_addr), ntohs(clientInfo.sin_port));

    send(client, name, 100, 0);

    pthread_t sendMsg;
    if (pthread_create(&sendMsg, NULL, (void *) sendMessage, NULL) != 0) {
        printf ("Create pthread error!\n");
        exit(EXIT_FAILURE);
    }

    pthread_t getMsg;
    if (pthread_create(&getMsg, NULL, (void *) getMessage, NULL) != 0) {
        printf ("Create pthread error!\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        if(flagExit) {
            printf("\nBye\n");
            break;
        }
    }

    close(client);
}
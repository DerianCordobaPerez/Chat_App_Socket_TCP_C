#include "../lib/builder.h"
#include "../lib/server.h"

ClientNode *root, *current;

extern const void exitProgramServer(int signal) {
    ClientNode *node;
    while(root != NULL) {
        printf("\nClose socket: %i\n", root->socket);
        close(root->socket);
        node = root;
        root = root->next;
        free(node);
    }
    error("\n\"GoodBye\"\n");
}

extern const void sendMessageAllClients(ClientNode* client, char* buffer) {
    ClientNode *node = root->next;

    while(node != NULL) {
        // All clients except the one in the current session.
        if(client->socket != node->socket) {
            printf("Send to socket: %i: \"%s\"\n", node->socket, buffer);
            send(node->socket, buffer, 100, 0);
        }
        node = node->next;
    }

}

extern const void handlerClient(void *client) {
    char *name = (char*)memoryAllocation(50), *getBuffer = (char*)memoryAllocation(100), *sendBuffer = (char*)memoryAllocation(100);
    int flag = 0;
    ClientNode *node = (ClientNode*)client;

    // We initialize the client to the server.
    if(recv(node->socket, name, 100, 0) <= 0) {
       printf("Failed to start the client \"%s\" inside the server.", node->ip);
       flag++; 
    } else {
        strncpy(node->name, name, 100);
        printf("%s <%s><%i> join to chat.\n", node->name, node->ip, node->socket);
        sprintf(sendBuffer, "%s <%s> join to chat.\n", node->name, node->ip);
        sendMessageAllClients(node, sendBuffer);
    }

    for(;;) {
        if(flag) break;

        int receive = recv(node->socket, getBuffer, 100, 0);
        if(receive > 0) {
            if(!strlen(getBuffer)) continue;
            sprintf(sendBuffer, "%s: %s from: %s", node->name, getBuffer, node->ip);
        } else if(receive == 0 || !strcmp(getBuffer, "exit")) {
            printf("%s{%s}{%d} leave the chatroom.\n", node->name, node->ip, node->socket);
            sprintf(sendBuffer, "%s{%s} leave the chat.", node->name, node->ip);
            flag = 1;
        } else flag = 1;

        sendMessageAllClients(node, sendBuffer);
    }

    close(node->socket);
    if(node != current) {
        node->previuos->next = node->next;
        node->next->previuos = node->previuos;
    } else {
        current = node->previuos;
        current->next = NULL;
    }
    free(node);

}

extern const void builderServer(char* flag, int port) {

    // We create the general signal for the use of the exit function.
    signal(SIGINT, exitProgramServer);

    // Create socket
    int server = socket(AF_INET , SOCK_STREAM , 0), client = 0;
    if (server == -1) {
        printf("Fail to create a socket.");
        exit(EXIT_FAILURE);
    }

   // Socket information
    struct sockaddr_in serverInfo, clientInfo;
    int serverLength = sizeof(serverInfo), clientLength = sizeof(clientInfo);
    memset(&serverInfo, 0, serverLength);
    memset(&clientInfo, 0, clientLength);
    serverInfo.sin_family = PF_INET;
    serverInfo.sin_addr.s_addr = htonl(0xC0A80006);
    serverInfo.sin_port = htons(port);

    // Bind and Listen
    bind(server, (struct sockaddr *)&serverInfo, serverLength);
    listen(server, 5);

    // Print Server IP
    getsockname(server, (struct sockaddr*) &serverInfo, (socklen_t*) &serverLength);
    printf("Start Server on: %s:%d\n", inet_ntoa(serverInfo.sin_addr), htons(serverInfo.sin_port));

    // Initial linked list for clients
    root = newClient(server, inet_ntoa(serverInfo.sin_addr));
    current = root;

    for(;;) {
        client = accept(server, (struct sockaddr*) &clientInfo, (socklen_t*) &clientLength);

        // Print Client IP
        getpeername(client, (struct sockaddr*) &clientInfo, (socklen_t*) &clientLength);
        printf("Client %s:%d come in.\n", inet_ntoa(clientInfo.sin_addr), ntohs(clientInfo.sin_port));

        // Append linked list for clients
        ClientNode *node = newClient(client, inet_ntoa(clientInfo.sin_addr));
        node->previuos = current;
        current->next = node;
        current = node;

        pthread_t id;
        if (pthread_create(&id, NULL, (void *)handlerClient, (void *)node) != 0)
            error("Create pthread error!\n");
    }
}
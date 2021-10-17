#include "../lib/builder.h"
#include "../lib/server.h"

ClientNode *root, *current;

extern const void exitProgramServer(int signal) {
    ClientNode *node;
    while (root != NULL) {
        printf("\nClosing socket: %i\n", root->socket);
        
        // Close all socket include server
        close(root->socket);
        node = root;
        root = root->next;
        free(node);
    }
    error("\"GoodBye\"");
}

extern const void sendMessageAllClients(ClientNode* client, char* buffer) {
    ClientNode *node = root->next;
    while (node != NULL) {
        // All clients except itself.
        if (client->socket != node->socket) {
            printf("Send to socket %i: \"%s\" \n", node->socket, buffer);
            send(node->socket, buffer, 250, 0);
        }
        node = node->next;
    }

}

extern const void handlerClient(void *client) {
    int flag = 0;
    char *name = (char*)memoryAllocation(50);
    char *getBuffer = (char*)memoryAllocation(100);
    char *sendBuffer = (char*)memoryAllocation(200);
    ClientNode *node = (ClientNode *)client;

    // Naming
    if (recv(node->socket, name, 50, 0) <= 0 || strlen(name) < 2 || strlen(name) >= 50) {
        printf("%s didn't input name.\n", node->ip);
        flag = 1;
    } else {
        strncpy(node->name, name, 50);
        printf("%s(%s)(%d) join the chatroom.\n", node->name, node->ip, node->socket);
        sprintf(sendBuffer, "%s(%s) join the chatroom.", node->name, node->ip);
        sendMessageAllClients(node, sendBuffer);
    }

    // Conversation
    for(;;) {
        if (flag) break;

        int receive = recv(node->socket, getBuffer, 100, 0);
        
        if (receive > 0) {
            if (strlen(getBuffer) == 0) continue;

            sprintf(sendBuffer, "%s：%s from %s", node->name, getBuffer, node->ip);
        } else if (receive == 0 || strcmp(getBuffer, "exit") == 0) {
            printf("%s(%s)(%d) leave the chatroom.\n", node->name, node->ip, node->socket);
            sprintf(sendBuffer, "%s(%s) leave the chatroom.", node->name, node->ip);
            flag = 1;
        } else {
            printf("Fatal Error: -1\n");
            flag = 1;
        }
        sendMessageAllClients(node, sendBuffer);
    }

    // Remove Node
    close(node->socket);
    if (node == current) { // remove an edge node
        current = node->previuos;
        current->next = NULL;
    } else { // remove a middle node
        node->previuos->next = node->next;
        node->next->previuos = node->previuos;
    }
    free(node);

}

extern const void builderServer(char* flag, int port) {

    // We create the general signal for the use of the exit function.
    signal(SIGINT, exitProgramServer);

    // Create socket
    int server = socket(AF_INET , SOCK_STREAM , 0), client = 0;
    if (server == -1)
        error("Fail to create a socket.");

   // Socket information
    struct sockaddr_in serverInfo, clientInfo;
    int serverLength = sizeof(serverInfo), clientLength = sizeof(clientInfo);
    memset(&serverInfo, 0, serverLength);
    memset(&clientInfo, 0, clientLength);
    serverInfo.sin_family = AF_INET;
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
            error("Failed to create thread to get clients.");
    }
}
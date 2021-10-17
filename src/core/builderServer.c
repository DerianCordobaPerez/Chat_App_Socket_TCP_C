#include "../lib/builder.h"
#include "../lib/server.h"
#define MAX 100
#define BUFFER 1024

static _Atomic unsigned int quantity = 0;
static int id = 1;

Client *clients[MAX];

pthread_mutex_t clientsMutex = PTHREAD_MUTEX_INITIALIZER;

extern const void addClient(Client *client) {
    pthread_mutex_lock(&clientsMutex);

    for (int i = 0; i < MAX; ++i) {
        if (!clients[i]) {
            clients[i] = client;
            break;
        }
    }

    pthread_mutex_unlock(&clientsMutex);
}

extern const void deleteClient(int id) {
    pthread_mutex_lock(&clientsMutex);

    for (int i = 0; i < MAX; ++i) {
        if (clients[i]) {
            if (clients[i]->id == id) {
                clients[i] = NULL;
                break;
            }
        }
    }

    pthread_mutex_unlock(&clientsMutex);
}

extern const void sendMessageAllClients(int id, char *message) {
    pthread_mutex_lock(&clientsMutex);

    for (int i = 0; i < MAX; ++i) {
        if (clients[i]) {
            if (clients[i]->id != id) {
                if (write(clients[i]->sockfd, message, strlen(message)) < 0)
                    error("Error sending message.");
            }
        }
    }

    pthread_mutex_unlock(&clientsMutex);
}

extern void *handlerClient(void *clientArg) {
    char output[BUFFER], name[50];
    int flag = 0;

    quantity++;
    Client *client = (Client *)clientArg;

    // Name
    if (recv(client->sockfd, name, 32, 0) <= 0 || strlen(name) < 2 || strlen(name) >= 32 - 1) {
        printf("Didn't enter the name.\n");
        flag = 1;
    }
    else {
        strcpy(client->name, name);
        sprintf(output, "%s has joined\n", client->name);
        printf("%s", output);
        sendMessageAllClients(client->id, output);
    }

    bzero(output, BUFFER);

    for(;;) {
        if (flag) break;

        int receive = recv(client->sockfd, output, BUFFER, 0);
        if (receive > 0) {
            if (strlen(output) > 0) {
                sendMessageAllClients(client->id, output);
                stringFormat(output, strlen(output));
                printf("%s: %s\n", client->name, output);
            }
        }
        else if (receive == 0 || strcmp(output, "exit") == 0) {
            sprintf(output, "%s has left\n", client->name);
            printf("%s", output);
            sendMessageAllClients(client->id, output);
            flag = 1;
        }
        else {
            printf("FATAL ERROR\n");
            flag = 1;
        }

        bzero(output, BUFFER);
    }

    // Delete client from queue and yield thread
    close(client->sockfd);
    deleteClient(client->id);
    free(client);
    quantity--;
    pthread_detach(pthread_self());

    return NULL;
}

extern const void builderServer(int port, int count)
{
    int option = 1, server = 0, connection = 0;
    struct sockaddr_in serverAddr, clientAddr;
    pthread_t threadID;

    // Socket settings
    server = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("192.168.0.10");
    serverAddr.sin_port = htons(port);

    // Ignore pipe signals
    signal(SIGPIPE, SIG_IGN);

    if (setsockopt(server, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char *)&option, sizeof(option)) < 0)
        error("Failed to set socket.");

    // Bind
    if (bind(server, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
        error("Failed to bind socket.");

    // Listen
    if (listen(server, 10) < 0)
        error("Failed to listen socket.");

    printf("* CHATROOM BY DERIAN *\n");

    for(;;) {
        socklen_t clientLength = sizeof(clientAddr);
        connection = accept(server, (struct sockaddr *)&clientAddr, &clientLength);

        // Check if max clients is reached
        if ((quantity + 1) == MAX) {
            printf("Max clients reached.");
            close(connection);
            continue;
        }

        // Client settings
        Client *client = (Client *)malloc(sizeof(Client));
        client->address = clientAddr;
        client->sockfd = connection;
        client->id = id++;

        // Add client to the queue and fork thread
        addClient(client);
        if(pthread_create(&threadID, NULL, &handlerClient, (void *)client) != 0)
            error("Error creating thread to handle client.");

        // Reduce CPU usage
        sleep(1);
    }
}
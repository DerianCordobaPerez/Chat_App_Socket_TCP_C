#include "../lib/builder.h"
#include "../lib/server.h"
#define MAX 100
#define BUFFER 1024

static _Atomic unsigned int quantity = 0;
static int id = 1;

Client *clients[MAX];

pthread_mutex_t clientsMutex = PTHREAD_MUTEX_INITIALIZER;

const void getInformationClients(int id) {
    pthread_mutex_lock(&clientsMutex);

    int _Atomic position = -1;
    char *message = (char*)memoryAllocation(200);
    bzero(message, 200);

    for(int i = 0; i < MAX; ++i) {
        if(clients[i]) {
            if(clients[i]->id == id) {
                position = i;
                break;
            }
        }
    }

    if(position != -1) {
        for(int i = 0; i < MAX; ++i) {
            if(clients[i]) {
                if(clients[position]->id != clients[i]->id) {
                    sprintf(message, "Socket: %i => (%s) - (%s)", clients[i]->socket, clients[i]->name, inet_ntoa(clients[i]->address.sin_addr));
                    strcat(message, "\n");
                    if (write(clients[position]->socket, message, strlen(message)) < 0)
                        error("Error sending message.");
                }
            }
        }
    }

    free(message);

    pthread_mutex_unlock(&clientsMutex);
}

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
                if (write(clients[i]->socket, message, strlen(message)) < 0)
                    error("Error sending message.");
            }
        }
    }

    pthread_mutex_unlock(&clientsMutex);
}

extern void *handlerClient(void *clientArg) {
    char output[BUFFER], name[50];
    bool flag = false;

    quantity++;
    Client *client = (Client *)clientArg;

    // Name
    for(;;) {
        if (recv(client->socket, name, 50, 0) <= 0 || strlen(name) < 2 || strlen(name) >= 50 - 1 || strcmp(name, "NONE") == 0) {
            flag = true;
            sleep(1);
            continue;
        }
        break;
    }

    flag = false;
    strcpy(client->name, name);
    sprintf(output, "%s has joined\n", client->name);
    printf("%s", output);
    sendMessageAllClients(client->id, output);

    bzero(output, BUFFER);

    for(;;) {
        if (flag) break;

        int receive = recv(client->socket, output, BUFFER, 0);
        if (receive > 0) {
            if (strlen(output) > 0) {

                stringFormat(output, strlen(output));

                if(strcmp(output, "-info") == 0) {      
                    getInformationClients(client->id);
                    continue;
                }

                if(*output != '\\') {
                    sendMessageAllClients(client->id, output);
                    printf("(%s)-%s\n", inet_ntoa(client->address.sin_addr), output);
                }
            }
        }
        else if (strcmp(output, "exit") == 0 || receive == 0) {
            sprintf(output, "%s has left\n", client->name);
            printf("%s", output);
            sendMessageAllClients(client->id, output);
            flag = true;
        }
        else {
            printf("FATAL ERROR\n");
            flag = true;
        }

        bzero(output, BUFFER);
    }

    // Delete client from queue and yield thread
    close(client->socket);
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
        client->socket = connection;
        client->id = id++;

        // Add client to the queue and fork thread
        addClient(client);
        if(pthread_create(&threadID, NULL, &handlerClient, (void *)client) != 0)
            error("Error creating thread to handle client.");

        // Reduce CPU usage
        sleep(1);
    }
}
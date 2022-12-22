#include "../lib/builder.h"
#include "../lib/server.h"
#define MAX 100
#define BUFFER 1024

// Number of connected clients.
static _Atomic unsigned int quantity = 0;

// Client ID
static int id = 1;
bool exists = false;

// Client array.
Client *clients[MAX];

// Traffic light by mutex.
pthread_mutex_t clientsMutex = PTHREAD_MUTEX_INITIALIZER;

extern const void getInformationClients(void) {
    pthread_mutex_lock(&clientsMutex);

    for(int i = 0; i < MAX; ++i) {
        if(clients[i])
            printf("Client: %s - Socket: %i\n", clients[i]->name, clients[i]->socket);
    }

    pthread_mutex_unlock(&clientsMutex);
}

extern const void sendPrivateMessageClient(char *name, char *message) {
    pthread_mutex_lock(&clientsMutex);

    printf("\nPrivate message\n");

    for(int i = 0; i < MAX; ++i) {
        if(clients[i]) {
            if(strcmp(name, clients[i]->name) == 0) {
                if (write(clients[i]->socket, message, strlen(message)) < 0)
                    error("Error sending message.");
            }
        }
    }
    pthread_mutex_unlock(&clientsMutex);
}

extern const void getConnectionsClients(int id) {
    pthread_mutex_lock(&clientsMutex);

    int _Atomic position = -1;
    char *message = (char*)memoryAllocation(200);
    bzero(message, 200);

    // We look for if there is a client with the id passed 
    // as a parameter.
    for(int i = 0; i < MAX; ++i) {
        if(clients[i]) {
            if(clients[i]->id == id) {
                position = i;
                break;
            }
        }
    }

    // If there is a client.
    if(position != -1) {
        for(int i = 0; i < MAX; ++i) {
            if(clients[i]) {
                // Send a message to all connections.
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

extern const void existsClient(char *name) {
    pthread_mutex_lock(&clientsMutex);

    for (int i = 0; i < MAX; ++i) {
        if (clients[i]) {
            if (strcmp(name, clients[i]->name) == 0) {
                sendPrivateMessageClient("NONE", "name_in_use");
                printf("Existe el cliente\n");
                exists = true;
            }
        }
    }
    pthread_mutex_unlock(&clientsMutex);
}

extern void *handlerClient(void *clientArg) {
    char output[BUFFER], name[50];
    bool flag = false;

    // We increase the number of clients.
    quantity++;

    // We get the current client.
    Client *client = (Client *)clientArg;

    // It will be in infinite cycle as long as the name does not 
    // come with the correct format.
    flag = true;
    while(recv(client->socket, name, 50, 0) <= 0 || strlen(name) < 2 || strlen(name) >= 50 - 1 || strcmp(name, "NONE") == 0) {
        sleep(1);
    }
        
    existsClient(name);

    if(!exists) {
        strcpy(client->name, name);
        // We welcome the new client.
        sprintf(output, "%s has joined\n", client->name);
        printf("%s", output);
        sendMessageAllClients(client->id, output);
        bzero(output, BUFFER);
        flag = false;
    }

    for(;;) {
        if (flag) break;

        // We obtain the source address of the message obtained.
        int receive = recv(client->socket, output, BUFFER, 0);

        // If the information received is greater than 0.
        if (receive > 0) {
            if (strlen(output) > 0) {
                // Format the sent message.
                stringFormat(output, strlen(output));

                // If the message begins with the * character.
                // send a private message.
                if(*output == '*') {
                    // Gets the sent message in part.
                    char *nameMsg = strtok(output, ":"), *message = strtok(NULL, ":");
                    // We remove the first character from the name.
                    nameMsg++;
                    sendPrivateMessageClient(nameMsg, message);
                    continue;
                }

                // We show the information of the clients connected 
                // from the other end.
                if(strcmp(output, "-info") == 0) {      
                    getConnectionsClients(client->id);
                    continue;
                }

                // List all connected users.
                if(strcmp(output, "-list") == 0) {      
                    getInformationClients();
                    continue;
                }

                // Show the message if it is not a command.
                sendMessageAllClients(client->id, output);
                printf("(%s)-%s\n", inet_ntoa(client->address.sin_addr), output);
            }
        // If the string is output or the information is less than 0.
        } else if (strcmp(output, "exit") == 0 || receive == 0) {

            // We notify that the client has left the chat.
            sprintf(output, "%s has left\n", client->name);
            printf("%s", output);
            sendMessageAllClients(client->id, output);
            flag = true;
        } else
            flag = true;

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
    if (listen(server, count + 1) < 0)
        error("Failed to listen socket.");

    printf("*--CHATROOM BY DERIAN--*\n");

    for(;;) {
        socklen_t clientLength = sizeof(clientAddr);
        connection = accept(server, (struct sockaddr *)&clientAddr, &clientLength);

        // Check if max clients is reached
        if ((quantity) == count) {
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
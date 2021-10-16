#include "../lib/main.h"

extern const void error(char *message) {
    puts(message);
    exit(EXIT_FAILURE);
}

extern const void validateParams(int argc) {
    if(argc < 3 && argc > 4)
        error("The number of parameters passed does not match the required ones\nExpected format for server ./chat <flag> <port>\nExpected format for client ./chat <flag> <port> <destination>");
}

extern const void *memoryAllocation(int quantity) {
    void *pointer;

    if((pointer = (void*)malloc(quantity > 0 ? (quantity * sizeof(void)) : (sizeof(void)))) == NULL)
        error("Failed to allocate memory inside the pointer");

    return pointer;
}

extern const char* getIp(char *adapter) {
    int server = socket(AF_INET, SOCK_DGRAM, 0);
    struct ifreq ifr;
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name , adapter, IFNAMSIZ - 1);
    ioctl(server, SIOCGIFADDR, &ifr);
    close(server);
    return inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
}

extern const void printWithFormat() {
    printf("\r%s", "> ");
    fflush(stdout);
}

extern const void stringFormat(char *message, int length) {
    for(int i = 0; i < length; ++i) {
        if(message[i] == '\n') {
            message[i] = '\0';
            break;
        }
    }
}

extern ClientNode *newClient(int socket, char* ip) {
    ClientNode *client = (ClientNode*)memoryAllocation(0);
    client->socket = socket;
    client->previuos = client->next = NULL;
    
    client->ip = (char*)memoryAllocation(16);
    client->ip = strdup(ip);

    client->name = (char*)memoryAllocation(16);
    client->name = strdup("NONE");

    return client;
}
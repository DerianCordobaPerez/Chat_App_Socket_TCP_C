#include "../lib/builder.h"
#include "../lib/client.h"
#define LENGTH 1024

volatile sig_atomic_t flag = 0;
FILE *conversationFile = NULL;
int server = 0;
char *nameFile, name[50];

void setNameFile() {
	nameFile = (char*)memoryAllocation(50);
	strcpy(nameFile, "Conversation-");
	strcat(nameFile, name);
	strcat(nameFile, ".txt");
}

void saveFile(char *message) {
	if(conversationFile != NULL)
		fprintf(conversationFile, "%s\n", message);
}

extern const void exitProgram(int signal) {
    flag = 1;
}

extern const void sendMessage() {
    char message[LENGTH] = {};
	char buffer[LENGTH + 32] = {};

	for(;;) {
		printWithFormat();
		fgets(message, LENGTH, stdin);
		stringFormat(message, LENGTH);

		if (strcmp(message, "exit") == 0) break;
		else {
			if(*message == '\\') {
				fflush(stdin);
				fflush(stdout);
				printf("\nStarting command: %s\n", message);
				
				if(strcmp(message, "\\quit") == 0) {
					break;
				} else if(strcmp(message, "\\log") == 0) {
					if(conversationFile == NULL)
						conversationFile = fopen(nameFile, "a+");
					else
						printf("Saving of the conversation has still started.");

				} else if(strcmp(message, "\\nolog") == 0) {
					if(conversationFile != NULL) {
						fclose(conversationFile);
						printf("\nConversation saved.\n");
					} else
						printf("\nSaving of the conversation has not started yet.");
				}
			} else {
				// We save the conversation inside the file
				saveFile(message);

				// We send the messages to the corresponding clients.
				sprintf(buffer, "%s\n", message);
				send(server, buffer, strlen(buffer), 0);
			}
		}

		bzero(message, LENGTH);
		bzero(buffer, LENGTH + 32);
	}
	exitProgram(2);
}

extern const void getMessage() {
    char message[LENGTH] = {};
	for(;;) {
		int receive = recv(server, message, LENGTH, 0);
		if (receive > 0) {
			printf("%s", message);
			printWithFormat();

			// We save the conversation inside the file
			saveFile(message);
		}
		else if (receive == 0) break;

		memset(message, 0, sizeof(message));
	}
}

extern const void builderClient(char *destination, int port) {
    signal(SIGINT, exitProgram);

	printf("Enter name: ");
	fgets(name, 51, stdin);
	stringFormat(name, strlen(name));

	if (strlen(name) > 50 || strlen(name) < 2)
        error("Error the name does not meet the required parameters.");

	struct sockaddr_in serverAddr;

	// Socket settings
	server = socket(AF_INET, SOCK_STREAM, 0);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(destination);
	serverAddr.sin_port = htons(port);

	setNameFile();

	// Connect to Server
	int err;
	if ((err = connect(server, (struct sockaddr *)&serverAddr, sizeof(serverAddr))) == -1)
        error("Failed to connect socket.");

	// Send name
	send(server, name, 50, 0);

	printf("*--CHATROOM BY DERIAN--*\n");

	pthread_t sendMsg;
	if (pthread_create(&sendMsg, NULL, (void *)sendMessage, NULL) != 0)
        error("Error creating thread to send messages.");

	pthread_t getMsg;
	if (pthread_create(&getMsg, NULL, (void *)getMessage, NULL) != 0)
        error("Error creating thread to get messages.");

	for(;;) {
		if (flag) {
			printf("\n\"GoodBye\"\n");
			break;
		}
	}

	if(conversationFile != NULL) 
		fclose(conversationFile);

	close(server);
	free(nameFile);

}
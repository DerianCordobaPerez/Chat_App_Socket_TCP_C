#include "../lib/builder.h"
#include "../lib/client.h"
#define LENGTH 1024

// Flag accessed from all threads.
volatile sig_atomic_t flag = 0;

// FILE variable that we use to perform the actions of the conversation 
// file.
FILE *conversationFile = NULL;

// socket to which we will connect.
int server = 0;

// File name and client name
char *nameFile, name[50];

extern const void setNameFile(void) {
	nameFile = (char*)memoryAllocation(50);

	// We prepare the name of the file with the word 
	// conversation and the name of the client.
	strcpy(nameFile, "Conversation-");
	strcat(nameFile, name);
	strcat(nameFile, "-");
	strcat(nameFile, getCurrentTime());
	strcat(nameFile, ".txt");
}

extern const void setName(void) {
	printf("Enter name: ");
	if(fgets(name, 50, stdin) != NULL) {
		stringFormat(name, strlen(name));
		// We validate the length of the name entered.
		if (strlen(name) > 50 || strlen(name) < 2)
			error("Error the name does not meet the required parameters.");
		
		// We send the name to the server.
		send(server, name, 50, 0);
	} else
		printf("The name cannot be null.");
}

extern const void saveFile(char *message) {
	// Write to the file.
	if(conversationFile != NULL)
		fprintf(conversationFile, "%s\n", message);
}

extern const void exitProgram(int signal) {
    flag = 1;
}

extern const void executeCommand(char *buffer, char *command) {
	bzero(buffer, LENGTH + 32);
	sprintf(buffer, "%s\n", command);
	send(server, buffer, strlen(buffer), 0);
}

extern const void sendMessage() {
	char buffer[LENGTH + 32] = {};
    char message[LENGTH] = {};

	for(;;) {
		// Print >
		printWithFormat(name, server);

		// We ask the console for the message to send
		if(fgets(message, LENGTH, stdin) != NULL)
			stringFormat(message, LENGTH);

		// We verify that the message sent is the command \name.
		if(strcmp(message, "\\name") == 0) {
			printf("\nStarting command: %s\n", message);
			setName();
			continue;
		}

		// In case the customer has not yet been given a name, 
		// it will not be able to send messages.
		if(strcmp(name, "NONE") != 0) {

			// If the message sent is exit, we stop the loop.
			if (strcmp(message, "exit") == 0) break;

			// In case of another message, we will validate the options.
			else {

				// If the sent message starts with inverted pleca, 
				// it works as a command.
				if(*message == '\\') {
					//We clean the input and output buffers.
					fflush(stdin);
					fflush(stdout);
					printf("\nStarting command: %s\n", message);
					
					// If the command is \quit, we end the loop.
					if(strcmp(message, "\\quit") == 0) {
						break;

					// If the command is \log, the conversation starts to be saved.
					} else if(strcmp(message, "\\log") == 0) {
						// If the file is null we open it.
						if(conversationFile == NULL) {
							// Set name in file
							setNameFile();
							conversationFile = fopen(nameFile, "a+");
						} else
							printf("Saving of the conversation has still started.");

					// If the command is \nolog, we save the conversation.
					} else if(strcmp(message, "\\nolog") == 0) {
						if(conversationFile != NULL) {
							fclose(conversationFile);
							printf("\nConversation saved.\n");
						} else
							printf("\nSaving of the conversation has not started yet.");

					// If the command is \info, we ask for the information of the other extremes.
					} else if(strcmp(message, "\\info") == 0) {
						executeCommand(buffer, "-info");

					// If the command is \list, We show all connected clients.
					} else if(strcmp(message, "\\list") == 0) 
						executeCommand(buffer, "-list");

					// If the command is \message, We send a private message to the 
					// given recipient.
					else if(strcmp(message, "\\message") == 0) {
						char nameSmg[50], privateMsg[200];
						bzero(buffer, LENGTH + 32);
						
						printf("Enter private message: ");

						// We get the message to send.
						if(fgets(privateMsg, 200, stdin) != NULL)
							stringFormat(privateMsg, 200);

						// We obtain the name of the client to send the message.
						printf("Enter the recipient's name: ");
						if(fgets(nameSmg, 50, stdin) != NULL)
							stringFormat(nameSmg, 50);
						
						// Message we send privately.
						sprintf(buffer, "*%s:%s\n", nameSmg, privateMsg);
						send(server, buffer, strlen(buffer), 0);
					} else
						printf("\nThe command entered is missing from the default command list.\n");
					
				} else {
					// We save the conversation inside the file
					saveFile(message);

					// We send the messages to the corresponding clients.
					sprintf(buffer, "Send from client \"%s\": %s\n", name, message);
					send(server, buffer, strlen(buffer), 0);
				}
			}
		} else
			printf("\n\"You have to assign a name to be able to send messages.\nrun the command \\name\".\n\n");

		bzero(message, LENGTH);
		bzero(buffer, LENGTH + 32);
	}

	// We send it the SIGINT signal equivalent to Ctrl + C.
	exitProgram(2);
}

extern const void getMessage() {
    char message[LENGTH] = {};
	for(;;) {

		// We obtain the source address of the message obtained.
		int receive = recv(server, message, LENGTH, 0);

		// If the information received is greater than 0, 
		// we print the message.
		if (receive > 0) {

			if(strcmp(message, "name_in_use") == 0)
				error("The chosen name has already been selected.");

			printf("%s\n\n", message);
			printWithFormat(name, server);

			// We save the conversation inside the file
			saveFile(message);
		}
		else if (receive == 0) break;

		// We set all memory spaces to 0.
		memset(message, 0, sizeof(message));
	}
}

extern const void builderClient(char *destination, int port) {
    // We give the signal through the output function.
	signal(SIGINT, exitProgram);
	struct sockaddr_in serverAddr;

	// Socket settings
	server = socket(AF_INET, SOCK_STREAM, 0);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(destination);
	serverAddr.sin_port = htons(port);

	// Default value for name
	strcpy(name, "NONE");

	// Connect to Server
	int status;
	if ((status = connect(server, (struct sockaddr *)&serverAddr, sizeof(serverAddr))) == -1)
        error("Failed to connect socket.");

	// Send name
	send(server, name, 50, 0);

	printf("*--CHATROOM BY DERIAN--*\n");

	// Create the thread to send messages.
	pthread_t sendMsg;
	if (pthread_create(&sendMsg, NULL, (void *)sendMessage, NULL) != 0)
        error("Error creating thread to send messages.");

	// Create the thread to get messages.
	pthread_t getMsg;
	if (pthread_create(&getMsg, NULL, (void *)getMessage, NULL) != 0)
        error("Error creating thread to get messages.");

	// It will loop as long as the flag is false.
	for(;;) {
		if (flag) {
			printf("\n\"GoodBye\"\n");
			break;
		}
	}

	// If the file was never closed (saved), it will be closed.
	if(conversationFile != NULL) 
		fclose(conversationFile);

	close(server);
	free(nameFile);

}
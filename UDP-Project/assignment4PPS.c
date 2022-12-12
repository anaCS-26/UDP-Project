#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>

#include "pokemon.h"

#define SERVER_PORT 6000

//SERVER SIDE
int main() {
	int serverSocket;
	struct sockaddr_in serverAddr, clientAddr;
	int status, addrSize, bytesReceived;
	fd_set readfds, writefds;
	char buffer[20] = ""; //stores incoming string
	char *fileName;//the name of filename
	pthread_t t1; //initializing a thread
	count_t *c1 = (count_t*)malloc(sizeof(count_t)); //allocates memory for the struct
	c1->num_pokemon = 0; //initializing num_pokemon to 0
	char flag = 0;

	if(c1 == NULL){
		printf("Error with malloc\n");
		exit(EXIT_FAILURE);
	}

	//entering pokemon.csv
	printf("Enter the name of the file (q to quit): ");
	scanf("%ms", &fileName);

	while(1){
		//if able to read then continue
		c1->fd = fopen(fileName, "r");
		if(c1->fd != NULL){
			free(fileName);
			break;
		}
		//if told to quit, free everything and quit
		else if(strcmp(fileName, "q") == 0){
			free(fileName);
			free(c1);
			return(1);
		}
		//if name not found then ask user to enter the name of the file again
		else{
			printf("Pokemon file is not found. Please enter the name of the file again (q to quit).\n");
			free(fileName);
			scanf("%ms", &fileName);
		}
	}
 // Create the server socket
	serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (serverSocket < 0) {
		printf("*** SERVER ERROR: Could not open socket.\n");
		exit(-1);
	}
 // Setup the server address
	memset(&serverAddr, 0, sizeof(serverAddr)); // zeros the struct
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons((unsigned short) SERVER_PORT);
 // Bind the server socket
	status = bind(serverSocket,(struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if (status < 0) {
		printf("*** SERVER ERROR: Could not bind socket.\n");
		exit(-1);
	}
 // Wait for clients now
	while (1) {
		FD_ZERO(&readfds);
		FD_SET(serverSocket, &readfds);
		FD_ZERO(&writefds);
		FD_SET(serverSocket, &writefds);
		status = select(FD_SETSIZE, &readfds, &writefds, NULL, NULL);
		if (status == 0) { // Timeout occurred, no client ready
		}
		else if (status < 0) {
			printf("*** SERVER ERROR: Could not select socket.\n");
			exit(-1);
		}
		else {
			addrSize = sizeof(clientAddr);
			bytesReceived = recvfrom(serverSocket, buffer, sizeof(buffer), 0, (struct sockaddr *) &clientAddr, &addrSize);
			if (bytesReceived > 0) {
				buffer[bytesReceived] = '\0';
				printf("SERVER: Received client request: %s\n", buffer);
			}
			//-------------------------------------------------------------------
			//if the received buffer string is not c or d
			int i = 0;
			if(strcmp(buffer, "c") != 0 && strcmp(buffer, "d") != 0){
				//but c1->search inside buffer
				strcpy(c1->search, buffer);
				//and make sure to reqind the pokemon array
				rewind(c1->fd);

				//creating semaphores
				if(sem_init(&c1->mutex, 0, 1)){
					printf("Error: on semaphore init\n");
					exit(1);
				}

				//creating thread that will run the read_pokemon function
				pthread_create(&t1, NULL, read_pokemon, (void *)c1);
				pthread_join(t1, NULL);

				//destroying semaphore
				sem_destroy(&c1->mutex);

				printf("SERVER: Sending Pokemon Array to client\n");
				char pokemon_count[5] = "";
				//first sending amount of num_pokemon
				sprintf(pokemon_count, "%d", c1->num_pokemon);
				sendto(serverSocket, pokemon_count, sizeof(pokemon_count), 0, (struct sockaddr *) &clientAddr, addrSize);

				//then sending all the individual lines
				for(i = 0; i < c1->num_pokemon; i++){
					char line[100] = "";
					//converting each pokemon into a line and sending that line to client
					pokemon_to_line(line, &c1->pokemon_array[i], SEPARATOR);
					sendto(serverSocket, line, sizeof(line), 0, (struct sockaddr *) &clientAddr, addrSize);
				}
				flag = 1;
			}
			//if the client presses c
			else if(strcmp(buffer, "c") == 0){
				//make sure to free everything
				printf("SERVER: Freeing memory (if any) for new client\n\n");
				if(flag){
					free(c1->pokemon_array);
				}
				flag = 0;
				//and initializing num_pokemon to 0
				c1->num_pokemon = 0;
			}
			//if client presses d
			else if(strcmp(buffer, "d") == 0){
				//make sure to free everything
				printf("SERVER: Freeing memory (if any) and stopping server\n\n");
				//closes the main pokemon file and frees everything
				fclose(c1->fd);
				if(flag){
					free(c1->pokemon_array);
				}
				free(c1);
				//breaks the main while loop
				break;
			}
 		}
	}
	//stops the program
	return(1);
}

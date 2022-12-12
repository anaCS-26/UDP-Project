#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//includes all functions used in assignment 3
#include "pokemon.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 6000

//CLIENT SIDE
int main() {
	int clientSocket, addrSize, bytesReceived;
	struct sockaddr_in serverAddr;
	char buffer[20]; // stores sent and received data
	char a[20]; //stores user input
	count_t *c1 = (count_t*)malloc(sizeof(count_t)); //allocates memory for struct
	c1->pokemon_array = malloc(sizeof(Pokemon)+1); //allocates memory for pokemon_array
	c1->num_pokemon = 0; //initializing num_pokemon to 0
	pthread_t t2; //creating thread
	int querie = 0; //querie count
	int numOutput = 0; //number of output names collected from user
	char OutputLine[100] = ""; //The line used to concatinate the output file names
 // Create socket
	clientSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (clientSocket < 0) {
		printf("*** CLIENT ERROR: Could open socket.\n");
		exit(-1);
	}
 // Setup address
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	serverAddr.sin_port = htons((unsigned short) SERVER_PORT);
 // Go into loop to commuincate with server now
	while (1) {
		addrSize = sizeof(serverAddr);

		while(1){
			//while loop for show menu
			printf("Options:\na.Type Search\nb.Save Results\nc.Exit the Program\nd.Exit Client and End Server\n");
			scanf("%s", a);

			//if user selects type search
			if(strcmp(a, "a") == 0){
				int i = 0;
				//user input for type
				printf("Enter Type 1 of Pokemon: ");
				scanf("%20s", buffer);
				//sends input to server
				printf("CLIENT: Sending \"%s\" to server.\n", buffer);
				sendto(clientSocket, buffer, strlen(buffer), 0, (struct sockaddr *) &serverAddr, addrSize);

				//client will collect the number of pokemon collected from type search
		 		char pokemon_count[5];
		 		bytesReceived = recvfrom(clientSocket, pokemon_count, sizeof(pokemon_count), 0, (struct sockaddr *) &serverAddr, &addrSize);
				c1->num_pokemon = atoi(pokemon_count);
				//With the number collected, the pokemon_array will be re allocated to fit the size of all the pokemons
				c1->pokemon_array = realloc(c1->pokemon_array, (c1->num_pokemon+1)*sizeof(Pokemon));

				//for that number of pokemons, it will collect line  of description from the server
				for(i = 0; i < c1->num_pokemon; i++){
					//line will be blank before concatinating
					char line[100] = "";
					//receive the line from server
					bytesReceived = recvfrom(clientSocket, line, sizeof(line), 0, (struct sockaddr *) &serverAddr, &addrSize);
					if(bytesReceived > 0){
						line[bytesReceived] = '\0';
					}

					//line to pokemon will convert the line into a pokemon structure
					line_to_pokemon(line, &c1->pokemon_array[i], SEPARATOR);
				}

				//once every line is given to client, client will confirm array has been sent
				printf("CLIENT: Got back Pokemon Array from server.\n");
				//querie count is added
				querie++;

				//will show number of pokemon caught
				printf("CLIENT: %d Pokemons caught in total\n\n", c1->num_pokemon);

			}
			//if user chooses to save their results
			else if(strcmp(a, "b") == 0){
				printf("------------------------\n");
				char outputName[20];
				//prompts user for output file name
				printf("Enter name of output file: ");
				scanf("%s", outputName);
				//it will write in the file if the name is valid otherwise it was ask user to enter name again
				while(1){
					c1->fout = fopen(outputName, "w");
					if(c1->fout != NULL){
						//OutputLine is concatinated to show at the end of program of all the output files made
						strcat(OutputLine, outputName);
						strcat(OutputLine, SEPARATOR);
						numOutput++;
						break;
					}
					else{
						//if output file could not be made then user enters file name again
						printf("Could not make output file. Enter name of file again: \n");
						scanf("%s", outputName);
					}
				}

				//creating thread 2 which will run the write_pokemon function
				pthread_create(&t2, NULL, write_pokemon, (void *)c1);

			}
			//if user chooses option c or d
			else if(strcmp(a, "c") == 0 || strcmp(a, "d") == 0){
				printf("CLIENT: Shutting down.\n");
				//shows querie count
				printf("CLIENT: Total Queries: %d\n", querie);
				//if there are any output files created it will show the line
				if(numOutput != 0){
					printf("CLIENT: Output files created: %s\n", OutputLine);
				}
				else{
					printf("CLIENT: No output files made\n");
				}
				//frees everything that was allocated
				free(c1->pokemon_array);
				free(c1);
				strcpy(buffer, a);
				//sends the option to server
				sendto(clientSocket, buffer, strlen(buffer), 0, (struct sockaddr *) &serverAddr, addrSize);

				close(clientSocket); // Don't forget to close the socket !
				//exiting all the threads in the background and exit program
				pthread_exit(NULL);
				return(1);
			}
			//if user enters options other than a,b,c,d
			else{
				printf("Not a valid option, please try again\n");
			}
		}
	}
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#include "pokemon.h"

/*
 * Function: read_pokemon
 * Description: Reads csv lines from a file, creates PokemonType structures,
 *              dymamically grows the array that holds them.
 * Parameters:
 *   void arg as this function is used for threads
 * Returns:
 *    Nothing
*/
void *read_pokemon(void *arg) {

	count_t *inc = (count_t *)arg;

	int num_lines = 0;

	char line[100] = "";
	char copy[100] = "";

	while (fscanf(inc->fd, "%[^\n]\n", line) != EOF) {
		strcpy(copy, line);
		if (num_lines > 0) {

			if(sem_wait(&inc->mutex)<0){
				printf("Error on semaphore wait\n");
				exit(1);
			}
			if(findType(line, inc->search) == 1){
				inc->num_pokemon++;
				if (inc->num_pokemon == 1) {

					inc->pokemon_array = calloc(1, sizeof(Pokemon));
      	}
      	else {
					inc->pokemon_array = realloc(inc->pokemon_array,(inc->num_pokemon+1)*sizeof(Pokemon));
      	}
				line_to_pokemon(copy, &inc->pokemon_array[inc->num_pokemon-1], SEPARATOR);
			}
			if(sem_post(&inc->mutex)<0){
					printf("Error on semaphore post\n");
					exit(1);
			}
		}
		num_lines++;
	}
	return 0;

}

/*
*Function: write_pokemon
*Description: Writes the lines for the output file from the array of pokemon information stored
*Parameters: void arg
*
*Returns: Nothing as this is a void pointer function
*/
void *write_pokemon(void *arg){

	count_t *inc = (count_t *)arg;

	for(int i = 0; i < inc->num_pokemon; i++){
		char line[100] = ""; /* Initialize the string to use strcat */
   	pokemon_to_line(line, &inc->pokemon_array[i], SEPARATOR);
 	 	fprintf(inc->fout, "%s\n", line);
	}
	fclose(inc->fout);
}

//This function is used to find the type1 from the stored pokemon information and match it with the given search from the user
int findType(char* line, char* search){
	char *value = strsep(&line, SEPARATOR);
	value = strsep(&line, SEPARATOR);
	value = strsep(&line, SEPARATOR);

	if(strcmp(search, value) == 0){
		return 1;
	}
	return 0;
}

/*
* Function: pokemon_to_line
* Description:
*   converts a PokemonType data structure to a csv string to be written to a file
* Parameters:
*   line_to_write: string to hold the PokemonType data
*   pokemon_to_write: pointer to the PokemonType structure that will populate the
*                     line_to_write string
*   separator: the separator character to use, ',' by default
*/
void pokemon_to_line(char* line_to_write, Pokemon* pokemon_to_write, char *separator) {

	char pokemon_id[5];
	sprintf(pokemon_id, "%d", pokemon_to_write->id);
	line_to_write = strcat(line_to_write, pokemon_id);
	line_to_write = strcat(line_to_write, separator);
  line_to_write = strcat(line_to_write, pokemon_to_write->name);
  line_to_write = strcat(line_to_write, separator);
  line_to_write = strcat(line_to_write, pokemon_to_write->type1);
  line_to_write = strcat(line_to_write, separator);
  line_to_write = strcat(line_to_write, pokemon_to_write->type2);
  line_to_write = strcat(line_to_write, separator);
  char total[5];
  sprintf(total, "%d", pokemon_to_write->total);
  line_to_write = strcat(line_to_write, total);
  line_to_write = strcat(line_to_write, separator);
  char hp[5];
  sprintf(hp, "%d", pokemon_to_write->hp);
  line_to_write = strcat(line_to_write, hp);
  line_to_write = strcat(line_to_write, separator);
  char attack[5];
  sprintf(attack, "%d", pokemon_to_write->attack);
  line_to_write = strcat(line_to_write, attack);
  line_to_write = strcat(line_to_write, separator);
  char defense[5];
  sprintf(defense, "%d", pokemon_to_write->defense);
  line_to_write = strcat(line_to_write, defense);
  line_to_write = strcat(line_to_write, separator);
  char spatk[5];
  sprintf(spatk, "%d", pokemon_to_write->spatk);
  line_to_write = strcat(line_to_write, spatk);
  line_to_write = strcat(line_to_write, separator);
  char spdef[5];
  sprintf(spdef, "%d", pokemon_to_write->spdef);
  line_to_write = strcat(line_to_write, spdef);
  line_to_write = strcat(line_to_write, separator);
  char speed[5];
  sprintf(speed, "%d", pokemon_to_write->speed);
  line_to_write = strcat(line_to_write, speed);
  line_to_write = strcat(line_to_write, separator);
  char generation[5];
  sprintf(generation, "%d", pokemon_to_write->generation);
  line_to_write = strcat(line_to_write, generation);
  line_to_write = strcat(line_to_write, separator);
  line_to_write = strcat(line_to_write, pokemon_to_write->legendary);

}

/*
* Function: line_to_pokemon
* Description: converts a csv line to a PokemonType data structure
* Parameters:
*    line: the line to be converted to a PokemonType
*    new_pokemon: the new PokemonType to be populated with data
*    separator: the character to use as the sparator, ',' by default
*/
void line_to_pokemon(char* line, Pokemon* new_pokemon, char* separator) {

	char *id = strsep(&line, separator);
	char *name = strsep(&line, separator);
	char *type1 = strsep(&line, separator);
	char *type2 = strsep(&line, separator);
	char *total = strsep(&line, separator);
	char *hp = strsep(&line, separator);
	char *attack = strsep(&line, separator);
	char *defense = strsep(&line, separator);
	char *spatk = strsep(&line, separator);
	char *spdef = strsep(&line, separator);
	char *speed = strsep(&line, separator);
	char *generation = strsep(&line, separator);
	char *legendary = strsep(&line, separator);

	new_pokemon->id = strcmp(id, "") ? atoi(id) : -1;
	strcpy(new_pokemon->name, name);
	strcpy(new_pokemon->type1, type1);
	strcpy(new_pokemon->type2, type2);
	new_pokemon->total = strcmp(total, "") ? atoi(total) : -1;
	new_pokemon->hp = strcmp(hp, "") ? atoi(hp) : -1;
	new_pokemon->attack = strcmp(attack, "") ? atoi(attack) : -1;
	new_pokemon->defense = strcmp(defense, "") ? atoi(defense) : -1;
	new_pokemon->spatk = strcmp(spatk, "") ? atoi(spatk) : -1;
	new_pokemon->spdef = strcmp(spdef, "") ? atoi(spdef) : -1;
	new_pokemon->speed = strcmp(speed, "") ? atoi(speed) : -1;
	new_pokemon->generation = strcmp(generation, "") ? atoi(generation) : -1;
	strcpy(new_pokemon->legendary, legendary);

}

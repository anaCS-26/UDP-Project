
#ifndef HEADER_POKEMON
#define HEADER_POKEMON

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define SEPARATOR ","

typedef struct PokemonType{
	int id;
	char name[50];
	char type1[20];
	char type2[20];
	int total;
	int hp;
	int attack;
	int defense;
	int spatk;
	int spdef;
	int speed;
	int generation;
	char legendary[5];
} Pokemon;

typedef struct count_t{
	FILE* fd;
  char search[20];
	Pokemon* pokemon_array;
	volatile int num_pokemon;

	FILE* fout;

	sem_t mutex;

}count_t;

void *read_pokemon(void *arg);
void *write_pokemon(void *arg);
void line_to_pokemon(char* line, Pokemon* new_pokemon, char *separator);
int findType(char* line, char* search);
void pokemon_to_line(char* line_to_write, Pokemon* pokemon_to_write, char *separator);

#endif

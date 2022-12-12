myExec: assignment4PPC.o assignment4PPS.o pokemon.o
	gcc -o ps assignment4PPS.o pokemon.o -lpthread
	gcc -o pc assignment4PPC.o pokemon.o -lpthread

assignment4PPC.o : assignment4PPC.c pokemon.h
	gcc -c assignment4PPC.c

assignment4PPS.o : assignment4PPS.c pokemon.h
	gcc -c assignment4PPS.c

pokemon.o : pokemon.c pokemon.h
	gcc -c pokemon.c

clean:
	rm -f *.o

CFLAGS=-std=c99 -Wall -pedantic -g
CC=gcc
RM=rm

projekt: main.o lex_an.o
	$(CC) main.o lex_an.o -o projekt

clean:
	$(RM) *.o

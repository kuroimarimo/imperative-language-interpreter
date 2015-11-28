CFLAGS=-std=c99 -Wall -pedantic -g
CC=gcc
RM=rm

projekt: main.o lex_an.o error.o parser.o ial.o prsa.o
	$(CC) main.o lex_an.o error.o parser.o ial.o prsa.o -o projekt

clean:
	$(RM) *.o

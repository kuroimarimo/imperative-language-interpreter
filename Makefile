CFLAGS=-std=c99 -Wall -pedantic -g
CC=gcc
RM=rm

projekt: main.o lex_an.o error.o parser.o ial.o prsa.o table_stack.o garbage_collector.o instructions.o frames.o interpret.o
	$(CC) main.o lex_an.o error.o parser.o ial.o prsa.o table_stack.o garbage_collector.o instructions.o frames.o interpret.o -o projekt

clean:
	$(RM) *.o

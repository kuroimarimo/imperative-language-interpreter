/*
 *  Project name:
 *  Implementace interpretu imperativního jazyka IFJ15
 *
 *  Date: 13.12.2015
 *
 *  Repository:
 *  https://github.com/kuroimarimo/imperative-language-interpreter
 *
 *  Team:
 *  Votjěch Václavík	(xvacla22)
 *  Peter Vančo			(xvanco05)
 *  Filip Vaško         (xvasko10)
 *  Dominik Vašek		(xvasek06)
 *  Valentína Straková	(xstrak27)
 */

#ifndef HASH_STACK
#define HASH_STACK

#include "ial.h"
#include "garbage_collector.h"

#define TABLE_SIZE	16

// Functions for work with stack of symbol tables

typedef struct
{
	hTab ** elems;
	int top, size;
} tableStack;

hTab * globalST;									// global symbol table
tableStack *localSTstack;							// stack of local symbol tables

tableStack * tableStackInit(int size);
int tableStackEmpty(tableStack * stack);
hTab * tableStackTop(tableStack * stack);
void tableStackPop(tableStack * stack);
hTab * tableStackPush(tableStack * stack, hTab * table);
void tableStackDispose(tableStack * stack);
hTab * getTableStackElem(tableStack * stack, int index);
void tableStackResize(tableStack * stack);

#endif

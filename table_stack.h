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

tableStack * tableStackInit(int size);
int tableStackEmpty(tableStack * stack);
hTab * tableStackTop(tableStack * stack);
void tableStackPop(tableStack * stack);
hTab * tableStackPush(tableStack * stack, hTab * table);
void tableStackDispose(tableStack * stack);
hTab * getTableStackElem(tableStack * stack, int index);

#endif

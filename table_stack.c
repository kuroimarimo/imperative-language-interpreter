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

#include "table_stack.h"

tableStack * tableStackInit(int size)
{
	tableStack * stack;

	stack = customMalloc(sizeof(tableStack));

	stack->top = -1;
	stack->size = size;
	stack->elems = customMalloc(size * sizeof(hTab*));
	
	for (int i = 0; i < stack->size; i++)
			stack->elems[i] = NULL;
		
	return stack;
}

int tableStackEmpty(tableStack * stack)
{
	return (stack->top < 0);
}

hTab * tableStackTop(tableStack * stack)
{
	if (stack->top >= 0)
		return stack->elems[stack->top];
	else
		return NULL;
}

void tableStackPop(tableStack * stack)
{
	if (tableStackEmpty(stack))
		return;
	else
		--stack->top;
}

hTab * tableStackPush(tableStack * stack, hTab * table)
{
	if (stack->top >= (stack->size - 1))		//stack is full
		tableStackResize(stack);
	
	stack->elems[stack->top + 1] = hTabInit(TABLE_SIZE);
	
	++stack->top;
	return stack->elems[stack->top];
}

void tableStackResize(tableStack * stack)
{
	stack->elems = customRealloc(stack->elems, 2 * stack->size * sizeof(hTab*));
	stack->size *= 2;
}

void tableStackDispose(tableStack * stack)
{
	for (int i = 0; i <= stack->top; i++)
		stack->elems[i] = NULL;                 // removes working references to symbol tables
}

hTab * getTableStackElem(tableStack * stack, int index)
{
	if ( (index >= 0) && (index <= stack->top) )
		return stack->elems[index];
	else
		return NULL;
}

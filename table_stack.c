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
		hTabFree(stack->elems[stack->top--]);
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
		hTabFree(stack->elems[i]);
}

hTab * getTableStackElem(tableStack * stack, int index)
{
	if ( (index >= 0) && (index <= stack->top) )
		return stack->elems[index];
	else
		return NULL;
}

#include "table_stack.h"

tableStack * tableStackInit(int size)
{
	tableStack * stack;

	if ((stack = malloc(sizeof(tableStack))) == NULL)
		return NULL;

	stack->top = -1;
	stack->size = size;
	if ((stack->elems = malloc(size * sizeof(hTab*))) == NULL)
    {
        free(stack);
        return NULL;
    }

	else
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
	if (stack->top == (stack->size - 1))		//stack is full
	{
		void * temp = realloc(stack->elems, stack->size * 2);
		if (temp == NULL)
			return NULL;
		
		stack->elems = temp;
		stack->size *= 2;
	}
	
	if ((stack->elems[stack->top + 1] = hTabInit(TABLE_SIZE)) == NULL)
		return NULL;
	
	++stack->top;
	return stack->elems[stack->top];
}

void tableStackDispose(tableStack * stack)
{
	for (int i = 0; i <= stack->top; i++)
		hTabFree(stack->elems[i]);

	free(stack->elems);
	free(stack);
}

hTab * getTableStackElem(tableStack * stack, int index)
{
	if ( (index >= 0) && (index <= stack->top) )
		return stack->elems[index];
	else
		return NULL;
}

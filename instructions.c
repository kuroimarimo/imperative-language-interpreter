#include "instructions.h"
#include "garbage_collector.h"
#include "ial.h"

//--------------------------//
//     ISNTRUCTION ARRAY    //
//--------------------------//

/* Dynamically expands the array where the instructions are stored.
Returns true upon successful completion, else returns false. (realloc failed) */
/*bool expandInstrList()
{
	int size = instructionList->lenght;
	instructionList->array = customRealloc(instructionList->array, size * 2 * sizeof(tInstruction));
	return instructionList->array == NULL;
}*/


/* Adds instruction to the global instruction list.
Returns true upon successful completion, else returns false. (realloc failed) */
bool addInstruction(tInstruction * instr)
{
	/*if (instructionList->lenght < (instructionList->occupied + 1) && !expandInstrList())
		return false;
	instructionList->array[instructionList->occupied] = *instr;
	++instructionList->occupied;
	return true;*/

	instr->next = NULL;

	if (instructionList->last == NULL)
		instructionList->first = instr;
	else
		instructionList->last->next = instr;

	instructionList->last = instr;
	return true;
}


/* Initialises the instruction list.
Returns true upon successful completion, else returns false. (malloc failed)*/
bool initInstrList()
{
	/*int size = INITIAL_SIZE;
	instructionList = customMalloc(sizeof(tInstrList));
	instructionList->array = customMalloc(size * sizeof(tInstruction));
	instructionList->lenght = size;
	instructionList->occupied = 0;*/

	instructionList = customMalloc(sizeof(tInstrList));
	instructionList->last = instructionList->first = NULL;
	return true;
}

void generateInstruction(int operation, void * input1, void * input2, void * output)		//generates instruction for non-aritmetic operations
{
	tInstruction * instruction = customMalloc(sizeof(tInstruction));
	instruction->operator = operation;
	instruction->type = -1;


	instruction->input1 = input1;
	instruction->input2 = input2;
	instruction->output = output;
	
	addInstruction(instruction);
}

tInstrStack * instrStackInit(int size)
{
    tInstrStack * temp = customMalloc(sizeof(tInstrStack));
    temp->top = -1;
    temp->size = size;
    temp->elems = customMalloc(sizeof(tInstruction *) * size);
    
    return temp;
}

void instrStackResize(tInstrStack * stack)
{
    stack->elems = customRealloc(stack, 2 * stack->size * sizeof(tInstruction *));
    stack->size *= 2;
}

void instrStackPush(tInstrStack * stack, tInstruction * instr)
{
    if (stack->top >= (stack->size - 1))
        instrStackResize(stack);
    
    stack->elems[++stack->top] = instr;
}
tInstruction * instrStackPop(tInstrStack * stack)
{
    void * temp = stack->elems[stack->top];
    --stack->top;
    
    return temp;
}

bool instrStackEmpty(tInstrStack * stack)
{
    return stack->top < 0;
}

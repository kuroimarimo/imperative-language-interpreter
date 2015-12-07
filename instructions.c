#include "instructions.h"
#include "garbage_collector.h"
#include "ial.h"

//--------------------------//
//     ISNTRUCTION ARRAY    //
//--------------------------//

/* Dynamically expands the array where the instructions are stored.
Returns true upon successful completion, else returns false. (realloc failed) */
bool expandInstrList()
{
	int size = instructionList->lenght;
	instructionList->array = customRealloc(instructionList->array, size * 2 * sizeof(tInstruction));
	return instructionList->array == NULL;
}


/* Adds instruction to the global instruction list.
Returns true upon successful completion, else returns false. (realloc failed) */
bool addInstruction(tInstruction * instr)
{
	if (instructionList->lenght < (instructionList->occupied + 1) && !expandInstrList())
		return false;
	instructionList->array[instructionList->occupied] = *instr;
	++instructionList->occupied;
	return true;
}


/* Initialises the instruction list.
Returns true upon successful completion, else returns false. (malloc failed)*/
bool initInstrList()
{
	int size = INITIAL_SIZE;
	instructionList = customMalloc(sizeof(tInstrList));
	instructionList->array = customMalloc(size * sizeof(tInstruction));
	instructionList->lenght = size;
	instructionList->occupied = 0;
	return true;
}
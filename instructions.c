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
 *  Vojtěch Václavík	(xvacla22)
 *  Petr Vančo			(xvanco05)
 *  Filip Vaško         (xvasko10)
 *  Dominik Vašek		(xvasek06)
 *  Valentína Straková	(xstrak27)
 */

#include "instructions.h"

//--------------------------//
//     ISNTRUCTION ARRAY    //
//--------------------------//

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

tInstruction * generateInstruction(int operation, void * input1, void * input2, void * output)		//generates instruction for non-aritmetic operations
{
    tInstruction * instruction = customMalloc(sizeof(tInstruction));
    instruction->operator = operation;
    
    instruction->input1 = input1;
    instruction->input2 = input2;
    instruction->output = output;
    
    addInstruction(instruction);
    return instructionList->last;
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

tVarCoordinates * constToVar(int constType, void * data)
{
    int * type = customMalloc(sizeof(int));
    tVarCoordinates * coordinates;
    void * outData;
    char str[512];
    char * constID;
    
    switch (constType)
    {
        case INT_NUMBER:
            sprintf(str, "#%d", *(int *)data);
            constID = strDuplicate(str);
            
            if (!findVar(constID))
                addVar(constID, getTableStackElem(localSTstack, 0), VAR_INT);
            
            coordinates = varToFrame(constID);
            *type = VAR_INT;
            outData = customMalloc(sizeof(int));
            *(int *)outData = *(int *)data;
            break;
            
        case DOUBLE_NUMBER:
            sprintf(str, "#%f", *(double *)data);
            constID = strDuplicate(str);
            //constID = concat("#", constID);
            
            if (!findVar(constID))
                addVar(constID, getTableStackElem(localSTstack, 0), VAR_DOUBLE);
            
            coordinates = varToFrame(constID);
            *type = VAR_DOUBLE;
            outData = customMalloc(sizeof(double));
            *(double *)outData = *(double *)data;
            break;
            
        case STRING:
            constID = strDuplicate(data);
            constID = concat("#", constID);
            
            if (!findVar(constID))
                addVar(constID, getTableStackElem(localSTstack, 0), VAR_STRING);
            
            coordinates = varToFrame(constID);
            *type = VAR_STRING;
            outData = strDuplicate(data);
            break;
    }
    
    generateInstruction(OP_CREATE_VAR, type, NULL, coordinates);
    generateInstruction(OP_SET_CONSTANT, outData, NULL, coordinates);
    /*     operand->type = expr->type;
     operand->operand = expr->data;*/
    
    return coordinates;
}

tVarCoordinates * tempToVar(int constType)
{
	int * type = customMalloc(sizeof(int));
	tVarCoordinates * coordinates;
	void * outData;
	char str[512];
	char * tempID;

	static int counter = 0;

	switch (constType)
	{
	case INT_NUMBER:
		sprintf(str, "$%d", counter++);
		tempID = strDuplicate(str);
		addVar(tempID, getTableStackElem(localSTstack, 0), VAR_INT);

		coordinates = varToFrame(tempID);
		*type = VAR_INT;
		outData = customMalloc(sizeof(int));
		break;

	case DOUBLE_NUMBER:
		sprintf(str, "$%d", counter++);
		tempID = strDuplicate(str);
		addVar(tempID, getTableStackElem(localSTstack, 0), VAR_DOUBLE);

		coordinates = varToFrame(tempID);
		*type = VAR_DOUBLE;
		outData = customMalloc(sizeof(double));
		break;

	case STRING:
		sprintf(str, "$%d", counter++);
		tempID = strDuplicate(str);
		addVar(tempID, getTableStackElem(localSTstack, 0), VAR_STRING);

		coordinates = varToFrame(tempID);
		*type = VAR_STRING;
		outData = NULL;
		break;
	}

	generateInstruction(OP_CREATE_VAR, type, NULL, coordinates);
	generateInstruction(OP_SET_CONSTANT, outData, NULL, coordinates);

	return coordinates;
}

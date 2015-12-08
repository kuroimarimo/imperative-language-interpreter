#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdbool.h>
#include <stdlib.h>
#include "3ak.h"

#define INITIAL_SIZE 512

// INSTRUCTION ARRAY
typedef struct
{
	int type;
	void * operand;
} tOperand;

typedef struct instr
{
	int operator;
	int type;
	tOperand *input1, *input2, *output;
	struct instr * next;
} tInstruction;

typedef struct
{
	/*int lenght;
	int occupied;
	tInstruction * array;*/
	tInstruction * first, *last;
} tInstrList;

tInstrList * instructionList;					//TODO separatne zoznamy instrukcii pre kazdu funkciu

bool expandInstrList();
bool addInstruction(tInstruction * instr);
bool initInstrList();
void debugInstrList();

void generateInstruction(int operation, void * input1, void * input2, void * output);

#endif

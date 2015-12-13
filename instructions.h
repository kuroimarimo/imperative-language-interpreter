#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdbool.h>
#include <stdlib.h>

#include "3ak.h"
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

#include "frames.h"
#include "table_stack.h"
#include "garbage_collector.h"
#include "ial.h"
#include "parser.h"

#define INITIAL_SIZE 512

// INSTRUCTION ARRAY
tInstrList * instructionList;

bool expandInstrList();
bool addInstruction(tInstruction * instr);
bool initInstrList();
void debugInstrList();

tInstruction * generateInstruction(int operation, void * input1, void * input2, void * output);

typedef struct {
    int top;
    int size;
    tInstruction ** elems;
} tInstrStack;

tInstrStack * instrStackInit(int size);
void instrStackResize(tInstrStack * stack);
void instrStackPush(tInstrStack * stack, tInstruction * instr);
tInstruction * instrStackPop(tInstrStack * stack);
bool instrStackEmpty(tInstrStack * stack);
tVarCoordinates * constToVar(int constType, void * data);
tVarCoordinates * tempToVar(int constType);

#endif

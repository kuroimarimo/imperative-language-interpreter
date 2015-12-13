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

#ifndef FRAMES_H
#define FRAMES_H

#include "ial.h"
#include "table_stack.h"
#include "garbage_collector.h"

typedef union unionValue
{
    int i;
    double d;
    char * s;
} TUnionValue;

typedef struct {
	bool initialized;
	TUnionValue /***/ value;
	int type;
} tVariable;

typedef struct {
	bool isBaseFrame;
	tVariable ** variables;
} tFrame;

typedef struct {
	int top;
	int size;
	tFrame ** frames;
} tFrameStack;

typedef struct					//coordinates of variable inside frame
{
	int frameOffset;			//how many levels under the frame stack top
	unsigned int index;					//index in the frame
} tVarCoordinates;

inline tFrame * frameCreate(int numberOfVars/*, bool isBase*/);
tFrameStack * frameStackInit(tFrameStack * stack, int size);

void frameCreateAndPush(tFrameStack * stack, int numOfVars/*, bool isBase*/);

inline void frameStackPush(tFrameStack * stack, tFrame * frame);
inline tFrame * frameStackTop(tFrameStack * stack);
void frameStackPop(tFrameStack * stack);
void frameStackPopUntilBase(tFrameStack * stack);							//pops all the frames to the first found base frame (including the base frame)

inline tFrame * getFrame(tFrameStack * stack, int index);							//returns frame >index< levels under top frame
tVariable * getVariable(tFrameStack * stack, tVarCoordinates * coordinates);	//return variable with index varIndex form frame frameIndex levels under framestack top	

void topToBase(tFrameStack * stack);										//sets the top frame as base frame
tVarCoordinates * varToFrame(char * key);
#endif

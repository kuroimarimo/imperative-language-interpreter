#ifndef FRAMES_H
#define FRAMES_H

#include "ial.h"
#include "table_stack.h"
#include "garbage_collector.h"

typedef struct {
	bool initialized;
	void * value;
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

tFrame * frameCreate(int numberOfVars/*, bool isBase*/);
tFrameStack * frameStackInit(tFrameStack * stack, int size);

void frameCreateAndPush(tFrameStack * stack, int numOfVars/*, bool isBase*/);

void frameStackPush(tFrameStack * stack, tFrame * frame);
tFrame * frameStackTop(tFrameStack * stack);
void frameStackPop(tFrameStack * stack);
void frameStackPopUntilBase(tFrameStack * stack);							//pops all the frames to the first found base frame (including the base frame)

tFrame * getFrame(tFrameStack * stack, int index);							//returns frame >index< levels under top frame
tVariable * getVariable(tFrameStack * stack, tVarCoordinates * coordinates);	//return variable with index varIndex form frame frameIndex levels under framestack top	

void topToBase(tFrameStack * stack);										//sets the top frame as base frame
tVarCoordinates * varToFrame(char * key);

tFrameStack * frameStack;

#endif

#ifndef FRAMES_H
#define FRAMES_H

#include "ial.h"
#include "table_stack.h"
#include "garbage_collector.h"

typedef struct {
	bool initialized;
	void * value;
	tSymbolType type;
} tVariable;

typedef struct{
	bool isBaseFrame;
	tVariable * variables;
} tFrame;

typedef struct {
	int top;
	int size;
	tFrame ** frames;
} tFrameStack;

typedef struct					//coordinates of variable inside frame
{
	int frameOffset;			//how many levels under the frame stack top
	int index;					//index in the frame
} tVarCoordinates;

tFrame * frameCreate(int numberOfVars/*, bool isBase*/);
void frameStackInit(tFrameStack * stack, int size);
void frameStackPush(tFrameStack * stack, tFrame * frame);
tFrame * frameStackTop(tFrameStack * stack);
void frameStackPop(tFrameStack * stack);
void frameStackPopUntilBase(tFrameStack * stack);							//pops all the frames to the first found base frame (including the base frame)
tFrame * getFrame(tFrameStack * stack, int index);							//returns frame >index< levels under top frame
tVariable * getVariable(tFrameStack * stack, tVarCoordinates * coordinates);	//return variable with index varIndex form frame frameIndex levels under framestack top	
void frameCreateAndPush(tFrameStack * stack, int numOfVars/*, bool isBase*/);

tVarCoordinates * varToFrame(char * key);

#endif

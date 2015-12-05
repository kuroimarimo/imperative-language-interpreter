#include "frames.h"

void frameStackInit(tFrameStack * stack, int size)
{
	stack = customMalloc(sizeof(tFrameStack));
	stack->frames = customMalloc(size * sizeof(tFrame*));
	stack->top = -1;
	stack->size = size;
}

tFrame * frameCreate(int numberOfVars, bool isBase)
{
	tFrame * temp = customMalloc(sizeof(tFrame));
	temp->variables = customMalloc(numberOfVars * sizeof(tVariable));
	temp->isBaseFrame = isBase;

	return temp;
}

void frameStackPush(tFrameStack * stack, tFrame * frame)
{
	if (stack->top == (stack->size - 1))
	{
		stack->frames = customRealloc(stack->frames, (stack->size * 2) * sizeof(tFrame*));
		stack->size *= 2;
	}

	stack->frames[++stack->top] = frame;
}

tFrame * frameStackTop(tFrameStack * stack)
{
	return stack->frames[stack->top];
}

tFrame * frameStackPop(tFrameStack * stack)
{
	--stack->top;
}

void frameStackPopUntilBase(tFrameStack * stack)
{
	while (!frameStackTop(stack)->isBaseFrame)		//pop all the non-base frames
		frameStackPop(stack);
	frameStackPop(stack);							//pop the base frame
}

tFrame * getFrame(tFrameStack * stack, int index)
{
	if (index > stack->top)
		return NULL;								//TODO fatalError

	return stack->frames[stack->top - index];
}

tVariable * getVariable(tFrameStack * stack, int frameIndex, int varIndex)
{
	return &(getFrame(stack, frameIndex)->variables[varIndex]);
}

void frameCreateAndPush(tFrameStack * stack, int numOfVars, bool isBase)
{
	frameStackPush(stack, frameCreate(numOfVars, isBase));
}
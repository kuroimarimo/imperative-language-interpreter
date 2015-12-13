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

#include "frames.h"

tFrameStack * frameStackInit(tFrameStack * stack, int size)
{
    stack = customMalloc(sizeof(tFrameStack));
    stack->frames = customMalloc(size * sizeof(tFrame*));
    stack->top = -1;
    stack->size = size;
    
    return stack;
}

tFrame * frameCreate(int numberOfVars/*, bool isBase*/)
{
    tFrame * temp = customMalloc(sizeof(tFrame));
    temp->variables = customMalloc(numberOfVars * sizeof(tVariable *));
    temp->isBaseFrame = /*isBase*/ false;								//frames aren't base frames by default, have to be changed by topToBase()
    
    for (int i = 0; i < numberOfVars; i++)
    {
        temp->variables[i] = customMalloc(sizeof(tVariable));
        temp->variables[i]->initialized = false;
        //temp->variables[i]->value = NULL;
    }
    
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

void frameStackPop(tFrameStack * stack)
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
    return stack->frames[stack->top - index];
}

tVariable * getVariable(tFrameStack * stack, tVarCoordinates * coordinates)
{
    return getFrame(stack, coordinates->frameOffset)->variables[coordinates->index];
}

void frameCreateAndPush(tFrameStack * stack, int numOfVars/*, bool isBase*/)
{
    tFrame * temp = frameCreate(numOfVars);
    frameStackPush(stack, temp);
}

tVarCoordinates * varToFrame(char * key)
{
    hashElem * temp = NULL;
    int i;
    
    
    for (i = localSTstack->top; i >= 0; i--)
        if ((temp = findElem(getTableStackElem(localSTstack, i), key)) != NULL)
            break;
    
    if (temp == NULL)
        return NULL;
    
    tVarCoordinates * coordinates = customMalloc(sizeof(tVarCoordinates));
    coordinates->frameOffset = localSTstack->top - i;
    coordinates->index = temp->data.index;
    
    
    return coordinates;
}

void topToBase(tFrameStack * stack)					//changes the frame on top of the stack to a base frame
{
    stack->frames[stack->top]->isBaseFrame = true;
}
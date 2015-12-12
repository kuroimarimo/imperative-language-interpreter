#ifndef GARBAGE_COLLECTOR_H
#define GARBAGE_COLLECTOR_H

#include <stdlib.h>
#include "error.h"

typedef struct garbage
{
	struct garbage * next;
	void * data;
} tGarbage;

typedef struct
{
	struct garbage * first;
	struct garbage * last;
} tGarbageList;

void   garbageListInit(tGarbageList * list);
void * appendGarbage(tGarbageList * list, void * data);
void   updateGarbageItem(tGarbageList * list, void * old, void * new);
void * customMalloc(int size);
void * customRealloc(void * data, int size);
void   garbageCleanup(tGarbageList * list);

tGarbageList garbageList;

#endif

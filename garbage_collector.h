#ifndef GARBAGE_COLLECTOR_H
#define GARBAGE_COLLECTOR_H

#include <stdlib.h>

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

void garbageListInit(tGarbageList * list);
void * appendGarbage(tGarbageList * list, void * data);
void   deleteGarbageItem(tGarbageList * list, void * deleted);
void * customMalloc(int size);
void * customRealloc(void * data, int size);
void garbageCleanup(tGarbageList * list);

tGarbageList garbageList;

#endif

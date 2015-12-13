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

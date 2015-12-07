#ifndef IAL_H
#define IAL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "instructions.h"
#include "garbage_collector.h"
#include "error.h"

typedef enum {VAR_INT, VAR_DOUBLE, VAR_STRING, FUNC_INT, FUNC_DOUBLE, FUNC_STRING} tSymbolType;
typedef union {int int_value; double double_value; char * string_value;} tSymbolValue;
typedef enum  {DECLARED, DEFINED} tState;

typedef struct
{
	tSymbolType type;
	char * key;
} tParam;

typedef struct data
{
    tSymbolType type;
    tState state;
    tSymbolValue value;					
    tParam * params;					// function parameters
	int numberOfParams;					// number of function parameters
	int index;							// index of the variable in variable frame
	//tInstrList * instructions;			// fuctions instruction list
} tData;

typedef struct hashElem
{
    struct hashElem * next; // in case of collision - pointer to the next element
    char * key;             // key of the element
    tData data;
} hashElem;

typedef struct hTab
{
    unsigned int size;          // size of hash table
    unsigned int numStoredElem; // number of currently stored elements
    hashElem ** table;          // the table itself
} hTab;

unsigned int hFunct(const char *str, unsigned hTabSize);

char * strDuplicate(char * src);
bool tDataCopy (tData * destData, tData * srcData);

hTab * hTabInit (unsigned int size);
void hashElemInit (hashElem * elem);		//initialises default hashElem values

bool checkOverfill (unsigned int size, unsigned int numStoredElem);
hTab * expandTab (hTab * table);

hashElem * addElem  (hTab * table, char * key, tData * data);
hashElem * findElem (hTab * table, char * key);

void removeElem (hTab * table, char * key);

void hTabFree (hTab * table);

int lenght(char *s);
char *substr (char *s, int i, int n);
int find(char*s, char*search);
char *concat (char *s1, char *s2);
char *sort(char *s);
char *sort_recursive(char *s, int left_poc, int right_poc);

#endif

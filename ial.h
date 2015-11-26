#ifndef IAL_H
#define IAL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef enum {var_int, var_double, var_string, func} tSymbolType;
typedef union {int int_value; double double_value; char * string_value;} tSymbolValue;
typedef enum  {declared, defined} tState;

typedef struct data
{
    tSymbolType type;
    tState state;
    tSymbolValue value;
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

hTab * hTabInit (unsigned int size);

bool checkOverfill (unsigned int size, unsigned int numStoredElem);
hTab * expandTab (hTab * table);

hashElem * addElem  (hTab * table, char * key, tData data);
hashElem * findElem (hTab * table, char * key);

void removeElem (hTab * table, char * key);

void hTabFree (hTab * table);

#endif

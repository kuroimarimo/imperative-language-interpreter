#ifndef PRSA_H
#define PRSA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include "3ak.h"
#include "ial.h"
#include "lex_an.h"
#include "error.h"
#include "garbage_collector.h"
#include "instructions.h"

#define NETERMINAL 500
#define TERMINAL 501
#define CALL_EXPRESSION 0
#define CALL_CONDITION 1

typedef struct {
    int terminal;
    int type;
    void *data;
} tExpr;

// ZASOBNIK
typedef struct tElem {      // deklarace struktury
    struct tElem *ptr;
    tExpr* data;
} *tElemPtr;                   
typedef struct {            // ukazatel prvniho prvku
    tElemPtr First;
} tList;

int PrecedencniSA (hTab *table, int PrecType);
void Dispose (tList *Z);

#endif

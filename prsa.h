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
#include "frames.h"
#include "table_stack.h"


#define NETERMINAL 500
#define TERMINAL 501
#define CALL_EXPRESSION 0
#define CALL_CONDITION 1
#define CALL_FOR 2

typedef struct {
    int terminal;
    int identifier;				//variable? 1/0
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

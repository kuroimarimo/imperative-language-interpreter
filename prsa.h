#ifndef PRSA_H
#define PRSA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include "ial.h"
#include "lex_an.h"
#include "error.h"
#include "parser.h"
#include "3ak.h"

#define NETERMINAL 500
#define TERMINAL 501

typedef union {
    int int_value;
    double double_value;
    char * string_value;
} tUnionData;

typedef struct
{
    int terminal;
    int type;
    tUnionData * data;
} tExpr;

// ZASOBNIK
typedef struct tElem {      // deklarace struktury
    struct tElem *ptr;
    tExpr * data;
} *tElemPtr;

typedef struct {            // ukazatel prvniho prvku
    tElemPtr First;
} tList;

// INSTRUCTION ARRAY

typedef struct
{
    int operator;
    int type;
    void *input1;
    void *input2;
    void *output;
} tInstruction;

typedef struct
{
    int lenght;
    int occupied;
    tInstruction * array;
} tInstrList;


// PRECEDENCNI TABULKA
char precedencni_tabulka[14][14] = {
//         *   /   +   -   >   >=  <   <=  ==  !=  (   )   i   ;
/* *  */ {'>','>','>','>','>','>','>','>','>','>','<','>','<','>'}, 
/* /  */ {'>','>','>','>','>','>','>','>','>','>','<','>','<','>'},
/* +  */ {'<','<','>','>','>','>','>','>','>','>','<','>','<','>'},
/* -  */ {'<','<','>','>','>','>','>','>','>','>','<','>','<','>'},
/* >  */ {'<','<','<','<','>','>','>','>','>','>','<','>','<','>'},
/* >= */ {'<','<','<','<','>','>','>','>','>','>','<','>','<','>'},
/* <  */ {'<','<','<','<','>','>','>','>','>','>','<','>','<','>'},
/* <= */ {'<','<','<','<','>','>','>','>','>','>','<','>','<','>'},
/* == */ {'<','<','<','<','>','>','>','>','>','>','<','>','<','>'},
/* != */ {'<','<','<','<','>','>','>','>','>','>','<','>','<','>'},
/* (  */ {'<','<','<','<','<','<','<','<','<','<','<','=','<',' '},
/* )  */ {'>','>','>','>','>','>','>','>','>','>',' ','>',' ','>'},
/* i  */ {'>','>','>','>','>','>','>','>','>','>',' ','>',' ','>'},
/* ;  */ {'<','<','<','<','<','<','<','<','<','<','<',' ','<',' '}
};

// GLOBALNA PREMENNA
tInstrList instructionList;

// OPERACE TABULKY
char OperatorToIndex (tExpr* op);

bool expandInstrList();
bool addInstruction(tInstruction * instr);
bool initInstrList();

int PrecedencniSA ();
void Dispose (tList *Z);
void DisposeList (tInstrList *L);

#endif

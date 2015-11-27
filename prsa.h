#ifndef PRSA_H
#define PRSA_H

#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>

#include "ial.h"
#include "lex_an.h"
#include "error.h"
#include "parser.h"
#include "3ak.h"

#define NETERMINAL 500
#define TERMINAL 501

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
// LINEARNI SEZNAM
typedef struct {
    int operator;
    int type;
    void *input1;
    void *input2;
    void *output;
} tInstruction;
typedef struct instructListElem {
    struct instructListElem *next;
    tInstruction *instruction;
} tInstrListElem;                  
typedef struct { 
    tInstrListElem *first;
    tInstrListElem *last;
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

// OPERACE TABULKY
char OperatorToIndex (tExpr* op) {
    switch (op->type) {
        case MULTIPLY:      return 0;   // *    = 0
        case DIVIDE:        return 1;   // /    = 1
        case PLUS:          return 2;   // +    = 2 
        case MINUS:         return 3;   // -    = 3
        case GREATER:       return 4;   // >    = 4
        case GREATER_ROVNO: return 5;   // >=   = 5
        case LESS:          return 6;   // <    = 6
        case LESS_ROVNO:    return 7;   // <=   = 7 
        case EQUAL:         return 8;   // ==   = 8
        case NEGACE:        return 9;   // !=   = 9
        case L_BRACKET:     return 10;  // (    = 10
        case R_BRACKET:     return 11;  // )    = 11
        case DOUBLE_NUMBER:
        case INT_NUMBER:
        case STRING:
        case IDENTIFIER:    return 12;  // i    = 12 <----- dodelat
        case SEMICOLON:     return 13;  // ;    = 13
        default:
            errorState.state = errcode;
            errorState.line = token.counter_of_lines;
            fatalError(errorState);
    }
}

int PrecedencniSA ();
void Dispose (tList *Z);
void InitList (tInstrList *L);
void DisposeList (tInstrList *L);
void InsertLast (tInstrList *L, tInstruction *data);
tInstruction* GetFirst (tInstrList *L);

#endif

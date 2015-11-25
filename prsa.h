#include<stdio.h>
#include<stdlib.h>
#include<stdarg.h>
#ifndef PRSA_H
#define PRSA_H

// ZASOBNIK
typedef struct tElem {		// deklarace struktury
    struct tElem *ptr;
    int data;
} *tElemPtr;	               
typedef struct { 			// ukazatel prvniho prvku
    tElemPtr First;
} tList;

// PRECEDENCNI TABULKA
char precedencni_tabulka[][] = {
//		   *   /   +   -   >   >=  <   <=  ==  !=  (   )   i   ;
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
}

// OPERACE TABULKY
char OperatorToIndex (char* op) {
	switch (op[0]) {
		case MULTIPLY: 		return 0;	// *	= 0
		case DIVIDE: 		return 1; 	// /	= 1
		case PLUS: 			return 2; 	// + 	= 2 
		case MINUS: 		return 3; 	// - 	= 3
		case GREATER: 		return 4;	// > 	= 4
		case GREATER_ROVNO:	return 5;	// >=	= 5
		case LESS: 			return 6;	// < 	= 6
		case LESS_ROVNO: 	return 7;	// <= 	= 7 
		case EQUAL: 		return 8; 	// == 	= 8
		case NEGACE: 		return 9; 	// != 	= 9
		case L_BRACKET: 	return 10; 	// ( 	= 10
		case R_BRACKET: 	return 11; 	// ) 	= 11
		case IDENTIFIER: 	return 12; 	// i 	= 12 <----- dodelat
		case SEMICOLON: 	return 13; 	// ; 	= 13
	}
}

int PrecedencniSA ();

#endif
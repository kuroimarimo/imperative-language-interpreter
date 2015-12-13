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

#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lex_an.h"
#include "error.h"
#include "ial.h"
#include "prsa.h"
#include "table_stack.h"
#include "garbage_collector.h"
#include "instructions.h"
#include "frames.h"
#include "interpret.h"

#define INIT_ST_SIZE	16							//initial table stack and symnbol table size
#define F_MAIN			"main"
#define DENY_PUSH		0
#define ALLOW_PUSH		1

tSymbolType functionType;							//return type of the currently processed function

//hashElem activeElem;								// symbol table element being modified
//tData STdata;										// tData being created


					/*Function definitions*/

int getType(int tokenType);							//converts keyword token type to tSymbolType for variables
int exprType(int type);								//converts number token type to tSymbolType for variables
int getFuncType(int tokenType);						//  -||- for functions									//merge var/func types?
void parse();										// parse the code
int isKeyword(int tokenType);						//checks whether the given token represents a keyword
hashElem * findVar(char *key);						// checks whether the symbol with given key exists anywhere on symbol table stack
													//and return fist found match
hashElem * isDeclaredOnTheSameLevel(char *key);		//looks for element with given key in the hashTable on top of table stack
hashElem * isFunct(char * key);						//checks whether there is a function with given key
hashElem * addVar(char * key, hTab * table, int type);				//adds variable into the given hashTable
int compareParams(hashElem * elem1, hashElem * elem2);				//compares parameters of given elements
int compareSymbol(hashElem * elem, hashElem * activeElem);			// compares 2 hashElem symbols
tParam * addParam(hashElem * elem, char * key, tSymbolType type);	//adds parameter to the parameter array
int convertType(tSymbolType inType, tSymbolType outType);			//type conversion
void processParam(int * paramIndex, hashElem * funcCall, int builtIn);
void checkFuncDefinitions();

																	
//functions simulating LL grammar rules             TODO

void rule_stList();									// <st-list> -> <statement> <st-list>     |   } 
void rule_statement(int pushAllowed);								// <statement> -> type id <var-def> ;   ||      id = <expression> ;      ||        id (<param-list>) ;      ||      <keyword>	|| { <st-list>
void rule_varDecl(hashElem * assignee);									// <var-decl> -> ;    |   = <expression> ;
void rule_funcdef(hashElem * activeElem);            // <prog> -> type id <param-list> <func-defined> 
void rule_funcDefined(hashElem * activeElem);        // <func-defined> -> { <st-list> <prog>     ||      ; <prog>
void rule_paramList(hashElem * activeElem);          // <param-list> -> ( <param> <param-next>
void rule_param(hashElem * activeElem);              // <param> -> type id
void rule_paramNext(hashElem * activeElem);          // <param-next> -> , <param> <param-next>       |   )
void rule_keyword();									// <keyword> -> auto <auto-decl>    ||      cin <cin> <cin-list>    ||  cout <cout> <cout-list>
													// ||   for <for-decl>  ||  if <if-decl>    ||  return <rule-return>	|| while <while_loop>	|| do <do-loop>
void rule_auto();									// <auto-decl> -> id = <expression>
int rule_cin();										// <cin> -> >> id
void rule_cinList();									// <cin-list> -> <cin> <cin-list>   ||  ;
int rule_cout();									// <cout> -> << id
void rule_coutList();								// <cout-list> -> <cout> <cout-list>   ||  ;
int rule_for();										// <for-decl> -> ( id <var-decl> <expression> id = <expression> ) <st-list>
int rule_if();										// <if-decl> -> ( <expression> ) <st-list> else <st-list>
void rule_return();									// <return> -> expression
int rule_while();									// <while-loop> -> ( <expression> ) <statement>
int rule_do();										// <do-loop> -> <statement> while ( <expression> )               

void rule_expression(hashElem * assignee);			//calls precedence SA for expressions or rule_function() for functions
int rule_funcCall(hashElem * assignee);
void rule_callParam(hashElem * funcCall, int  * paramIndex, int builtIn);
void rule_callParamList(hashElem * funcCall, int  * paramIndex, int builtIn);		//TODO

//rules for built-in functions
int rule_builtIn(hashElem * assignee);

#endif

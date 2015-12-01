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

#define INIT_ST_SIZE	16							//initial table stack and symnbol table size

FILE* srcFile;
hTab * globalST;									// global symbol table
tableStack *localSTstack;							// stack of local symbol tables
//hashElem activeElem;								// symbol table element being modified
//tData STdata;										// tData being created


					/*Function definitions*/

int getType(int tokenType);							//converts keyword token to tSymbolType for variables
int getFuncType(int tokenType);						//  -||- for functions									//merge var/func types?
int parse();										// parse the code
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

																	
//functions simulating LL grammar rules             TODO

int rule_stList();									// <st-list> -> <statement> <st-list>     |   } 
int rule_statement();								// <statement> -> type id <var-def> ;   ||      id = <expression> ;      ||        id (<param-list>) ;      ||      <keyword>	|| { <st-list>
int rule_varDecl(hashElem * assignee);									// <var-decl> -> ;    |   = <expression> ;
int rule_funcdef(hashElem * activeElem);            // <prog> -> type id <param-list> <func-defined> 
int rule_funcDefined(hashElem * activeElem);        // <func-defined> -> { <st-list> <prog>     ||      ; <prog>
int rule_paramList(hashElem * activeElem);          // <param-list> -> ( <param> <param-next>
int rule_param(hashElem * activeElem);              // <param> -> type id
int rule_paramNext(hashElem * activeElem);          // <param-next> -> , <param> <param-next>       |   )
int rule_keyword();									// <keyword> -> auto <auto-decl>    ||      cin <cin> <cin-list>    ||  cout <cout> <cout-list>
													// ||   for <for-decl>  ||  if <if-decl>    ||  return <rule-return>	|| while <while_loop>	|| do <do-loop>
int rule_auto();									// <auto-decl> -> id = <expression>
int rule_cin();										// <cin> -> >> id
int rule_cinList();									// <cin-list> -> <cin> <cin-list>   ||  ;
int rule_cout();									// <cout> -> << id
int rule_coutList();								// <cout-list> -> <cout> <cout-list>   ||  ;
int rule_for();										// <for-decl> -> ( id <var-decl> <expression> id = <expression> ) <st-list>
int rule_if();										// <if-decl> -> ( <expression> ) <st-list> else <st-list>
int rule_return();									// <return> -> expression
int rule_while();									// <while-loop> -> ( <expression> ) <statement>
int rule_do();										// <do-loop> -> <statement> while ( <expression> )               

int rule_expression(hashElem * assignee);			//calls precedence SA for expressions or rule_function() for functions
int rule_funcCall();
int rule_callParam(hashElem * funcCall);
int rule_callParamList(hashElem * funcCall);		//TODO

//rules for built-in functions
int rule_BLength();
int rule_BSubstr();
int rule_BConcat();
int rule_BFind();
int rule_BSort();

#endif

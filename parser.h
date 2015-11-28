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

#define INIT_ST_SIZE	16

hTab * globalST;				// global symbol table
hTab * localST;					// local symbol table for the currently parsed function
hashElem activeElem;			// symbol table element being modified
//tData STdata;					// tData being created

                                                    /*Function definitions*/

int parse(FILE *source);                    // parse the code
int isKeyword(int tokenType);


//functions simulating LL grammar rules             TODO

int rule_stList();              // <st-list> -> <statement> <st-list>     |   } 

int rule_statement();           // <statement> -> type id <var-def> ;   ||      id = <expression> ;      ||        id (<param-list>) ;      ||      <keyword>	|| { <st-list>
int rule_varDecl();             // <var-decl> -> ;    |   = <expression> ;

int rule_funcdef();             // <prog> -> type id <param-list> <func-defined> 
int rule_funcDefined();         // <func-defined> -> { <st-list> <prog>     ||      ; <prog>
int rule_paramList();           // <param-list> -> ( <param> <param-next>
int rule_param();               // <param> -> type id
int rule_paramNext();           // <param-next> -> , <param> <param-next>       |   )

int rule_keyword();             // <keyword> -> auto <auto-decl>    ||      cin <cin> <cin-list>    ||  cout <cout> <cout-list>
                                // ||   for <for-decl>  ||  if <if-decl>    ||  return <rule-return>	|| while <while_loop>	|| do <do-loop>

int rule_auto();                // <auto-decl> -> id = <expression>
int rule_cin();                 // <cin> -> >> id
int rule_cinList();             // <cin-list> -> <cin> <cin-list>   ||  ;
int rule_cout();                // <cout> -> << id
int rule_coutList();            // <cout-list> -> <cout> <cout-list>   ||  ;
int rule_for();                 // <for-decl> -> ( id <var-decl> <expression> id = <expression> ) <st-list>
int rule_if();                  // <if-decl> -> ( <expression> ) <st-list> else <st-list>
int rule_return();				// <return> -> expression
int rule_while();				// <while-loop> -> ( <expression> ) <statement>
int rule_do();					// <do-loop> -> <statement> while ( <expression> )               


int rule_expression();

int getType(int tokenType);
char * appendChar(char * string, char c);
char paramTypeToChar(int type);
int paramTypeToInt(char c);

int compareSymbol(hashElem * elem);                          //compares elem with global variable activeElem

#endif

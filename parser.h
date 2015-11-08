#ifndef PARSER_H
#define PARSER_H

                                                    /*Function definitions*/

int parse(FILE *source);                    // parse the code


//functions simulating LL grammar rules             TODO

int rule_stList();              // <st-list> -> <statement> ; <st-list>     |   } 

int rule_statement();           // <statement> -> type id <var-decl>    ||      id
int rule_varDecl();             // <var-decl> -> epsilon    |   <expression>

int rule_funcdef();             // <prog> -> type id <param-list> <func-defined> 
int rule_funcDefined();         // <func-defined> -> { <st-list> <prog>     ||      ; <prog>
int rule_paramList();           // <param-list> -> ( <param> <param-next>
int rule_param();               // <param> -> type id
int rule_paramNext();           // <param-next> -> , <param> <param-next>       |   )

#endif
#ifndef PARSER_H
#define PARSER_H

                                                    /*Function definitions*/

int parse(FILE *source);                    // parse the code


//functions simulating LL grammar rules             TODO

int rule_stList();              // <st-list> -> <statement> <st-list>     |   } 

int rule_statement();           // <statement> -> type id <var-def> ;   ||      id = <expression> ;      ||        id (<param-list>) ;      ||      <keyword>
int rule_varDecl();             // <var-decl> -> ;    |   = <expression> ;

int rule_funcdef();             // <prog> -> type id <param-list> <func-defined> 
int rule_funcDefined();         // <func-defined> -> { <st-list> <prog>     ||      ; <prog>
int rule_paramList();           // <param-list> -> ( <param> <param-next>
int rule_param();               // <param> -> type id
int rule_paramNext();           // <param-next> -> , <param> <param-next>       |   )

int rule_keyword();             // <keyword> -> auto <auto-decl>    ||      cin <cin> <cin-list>    ||  cout <cout> <cout-list>
                                // ||   for <for-decl>  ||  if <if-decl>    ||  return <rule-return>

int rule_auto();                // <auto-decl> -> id = <expression>
int rule_cin();                 // <cin> -> >> id
int rule_cinList();             // <cin-list> -> <cin> <cin-list>   ||  ;
int rule_cout();                // <cout> -> << id
int rule_coutList();            // <cout-list> -> <cout> <cout-list>   ||  ;
int rule_for();                 // <for-decl> -> ( id <var-decl> <expression> id = <expression> ) <st-list>
int rule_if();                  // <if-decl> -> ( <expression> ) <st-list> else <st-list>
int rule_return();				// <return> -> expression               


int rule_expression();

#endif
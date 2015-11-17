#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"
#include "lex_an.h"
#include "error.h"


FILE* srcFile;

int parse(FILE *source)
{
    srcFile = source;

    int returnValue = ERR_None;

    while ((returnValue == ERR_None))           //no errors so far
    {
        scanner(srcFile);                       //get next token
        if (token.type == EOF)
            break;

        returnValue = rule_funcdef(srcFile);        //start parsing recursively
    }

    return returnValue;
}

// rule:    <prog> -> type id <param-list> <func-defined> 
int rule_funcdef()
{
    if ((token.type =! KEY_WORD) || ( (strcmp(token.unie.area, "int") != 0) && (strcmp(token.unie.area, "double") != 0) && (strcmp(token.unie.area, "string") != 0)) )//((token.type =! KEY_WORD) || ( (token.type != INT) && (token.type != DOUBLE) && (token.type != STRING)) )
        return ERR_SYNTAX;

    //vytvorenie polozky TS pre funkciu
    
    scanner(srcFile);
    if (token.type != IDENTIFIER)
        return ERR_SYNTAX;

    //pridanie ID funkcie
    
    int error = rule_paramList();
    if (error != ERR_None)
        return error;

    error = rule_funcDefined();
    if (error != ERR_None)
        return error;


    return ERR_None;
}

//rule:     <func-defined> -> { <st-list> <prog>        ||      ; <prog>
int rule_funcDefined()
{
    scanner(srcFile);

    if (token.type == SEMICOLON)
        return ERR_None;

    if (token.type != L_CURLY_BRACKET)
        return ERR_SYNTAX;

    int error = rule_stList();
    if (error != ERR_None)
        return error;

    return ERR_None;
}

//rule:     <param-list> -> ( <param> <param-next>      |       ()
int rule_paramList()
{
    scanner(srcFile);
    if (token.type != L_BRACKET)
        return ERR_SYNTAX;

    scanner(srcFile);
    if (token.type == R_BRACKET)
        return ERR_None;

    int error = rule_param();
    if (error != ERR_None)
        return error;

    error = rule_paramNext();
    if (error != ERR_None)
        return error;

    return ERR_None;
}

//rule:     <param> -> type id
int rule_param()    
{
    if ((token.type =! KEY_WORD) || ( (strcmp(token.unie.area, "int") != 0) && (strcmp(token.unie.area, "double") != 0) && (strcmp(token.unie.area, "string") != 0)) )//((token.type =! KEY_WORD) || ( (token.type != INT) && (token.type != DOUBLE) && (token.type != STRING)))
        return ERR_SYNTAX;

    // ulozenie typu parametra do tabulky

    scanner(srcFile);
    if (token.type != IDENTIFIER)
        return ERR_SYNTAX;

    //pridanie ID premennej do tabulky

    return ERR_None;
}

//rule:     <param-next> -> , <param> <param-next>      |   )
int rule_paramNext()
{
    scanner(srcFile);
    
    if (token.type == COMMA)
    {

        scanner(srcFile);
        int error = rule_param();
        if (error != ERR_None)
            return error;

        error = rule_paramNext();
        if (error != ERR_None)
            return error;

        return ERR_None;
    }

    else 
    {
        if (token.type == R_BRACKET)
            return ERR_None;
    }

    return ERR_SYNTAX;
}   

//rule:     <st-list> -> <statement> ; <st-list>    |   }
int rule_stList()
{
    int error;

    scanner(srcFile);

    if (token.type == R_CURLY_BRACKET)
        return ERR_None;

    error = rule_statement();
        if (error != ERR_None)
            return error;

    error = rule_stList();
        if (error != ERR_None)
            return error;

    return ERR_None;
}

//rule:     <statement> -> type id <var-decl> ;   ||      id <assignment> ;      ||        id (<param-list> ;       ||      <keyword>   || { <st-list>
int rule_statement()
{

    int error = ERR_SYNTAX;

    if ((token.type == KEY_WORD) && ( (strcmp(token.unie.area, "int") == 0) || (strcmp(token.unie.area, "double") == 0) || (strcmp(token.unie.area, "string") == 0)) )
    {
        scanner(srcFile);

        if (token.type != IDENTIFIER)
            return ERR_SYNTAX;
        error = rule_varDecl();
    }

    else 
        if (token.type == IDENTIFIER)
        {
            scanner(srcFile);
            if (token.type == ASSIGNMENT)
                error = rule_expression();
        }

        else if (token.type == KEY_WORD)
        {
            return rule_keyword();
        }

        else if (token.type == L_CURLY_BRACKET)
            error = rule_stList();

    return error;
}

//rule:     <var-decl> -> ;    |   = <expression> ;
int rule_varDecl()
{
    scanner(srcFile);

    if (token.type == SEMICOLON)
        return ERR_None;

    if (token.type == ASSIGNMENT)
        return (rule_expression());

    return ERR_SYNTAX;
}

int rule_expression()
{
    scanner(srcFile);
    if (strcmp(token.unie.area, "__vyraz__") != 0)
        return ERR_SYNTAX;

    scanner(srcFile);
    if (token.type != SEMICOLON)
        return ERR_SYNTAX;

    return ERR_None;
}

//rule:     <auto-decl> -> id = <expression>
int rule_auto()
{
    scanner(srcFile);
    if (token.type != IDENTIFIER)
        return ERR_SYNTAX;

    scanner(srcFile);
    if (token.type != ASSIGNMENT)
        return ERR_SYNTAX;

    return rule_expression();
} 

//rule:     <cin> -> >> id
int rule_cin()
{
    if (token.type != C_IN)
        return ERR_SYNTAX;

    scanner(srcFile);
    if (token.type != IDENTIFIER)
        return ERR_SYNTAX;

    return ERR_None;
}

// <cin-list> -> <cin> <cin-list>   ||  ;
int rule_cinList()
{
    scanner(srcFile);

    if (token.type == SEMICOLON)
        return ERR_None;
    else
    {
        int error = rule_cin();
        if (error != ERR_None)
            return error;
        else 
            return (rule_cinList());
    }
}

//rule:     <cout> -> << id
int rule_cout()
{
    if (token.type != C_OUT)
        return ERR_SYNTAX;

    scanner(srcFile);
    if ((token.type != IDENTIFIER) && (token.type != STRING) && (token.type != INT) && (token.type != DOUBLE))
        return ERR_SYNTAX;

    return ERR_None;
}

// <cout-list> -> <cout> <cout-list>   ||  ;
int rule_coutList()
{
    scanner(srcFile);

    if (token.type == SEMICOLON)
        return ERR_None;
    else
    {
        int error = rule_cout();
        if (error != ERR_None)
            return error;
        else 
            return (rule_coutList());
    }
}

//rule:     <if-decl> -> ( <expression> ) <st-list> else <st-list>
int rule_if()
{
    scanner(srcFile);
    if (token.type != L_BRACKET)
        return ERR_SYNTAX;

    int error = rule_expression();
    if (error != ERR_None)
        return error;

    scanner(srcFile);
    if (token.type != R_BRACKET)
        return ERR_SYNTAX;

    scanner(srcFile);
    if (token.type != L_CURLY_BRACKET)
        return ERR_SYNTAX;

    if ((error = rule_stList()) != ERR_None)
        return error;

    scanner(srcFile);
    if (strcmp(token.unie.area, "else") != 0)
        return ERR_SYNTAX;

    scanner(srcFile);
    if (token.type != L_CURLY_BRACKET)
        return ERR_SYNTAX;

    if ((error = rule_stList()) != ERR_None)
        return error;

    return ERR_None;
}

//rule:     <return> -> expression
int rule_return()
{
    return rule_expression();
}

//rule:     <for-decl> -> ( id <var-decl> <expression> id = <expression> ) { <st-list>
int rule_for()
{
    scanner(srcFile);
    if (token.type != L_BRACKET)
        return ERR_SYNTAX;

    scanner(srcFile);
    if ((token.type =! KEY_WORD) || ( (strcmp(token.unie.area, "int") != 0) && (strcmp(token.unie.area, "double") != 0) && (strcmp(token.unie.area, "string") != 0)) )//((token.type =! KEY_WORD) || ( (token.type != INT) && (token.type != DOUBLE) && (token.type != STRING)) )
        return ERR_SYNTAX;

    scanner(srcFile);
    if (token.type != IDENTIFIER)
        return ERR_SYNTAX;

    int error;

    if ((error = rule_varDecl()) != ERR_None)
        return error;

    if ((error = rule_expression()) != ERR_None)
        return error;

    scanner(srcFile);
    if (token.type != IDENTIFIER)
        return ERR_SYNTAX;

    scanner(srcFile);
    if (token.type != ASSIGNMENT)
        return ERR_SYNTAX;

    if ((error = rule_expression()) != ERR_None)
        return error;

    scanner(srcFile);
    if (token.type != R_BRACKET)
        return ERR_SYNTAX;

    scanner(srcFile);
    if (token.type != L_CURLY_BRACKET)
        return ERR_SYNTAX;

    return rule_stList();
}


//rule:     <keyword> -> auto <auto-decl>    ||      cin <cin> <cin-list>    ||  cout <cout> <cout-list>     ||   for <for-decl>  ||  if <if-decl>    ||  return <expression>

int rule_keyword()
{
    int error;

    if (strcmp(token.unie.area, "cin") == 0)
    {

        scanner(srcFile);
        if ((error = rule_cin()) != ERR_None)
            return error;
        else 
            return rule_cinList();
    }

    else if (strcmp(token.unie.area, "cout") == 0)
    {
        scanner(srcFile);

        if ((error = rule_cout()) != ERR_None)
            return error;
        else 
            return rule_coutList();
    }

    else if (strcmp(token.unie.area, "if") == 0)
    {
        return rule_if();
    }

    else if (strcmp(token.unie.area, "for") == 0)
    {
        return rule_for();
    }

    else if (strcmp(token.unie.area, "return") == 0)
    {
        return rule_return();
    }

    else if (strcmp(token.unie.area, "auto") == 0)
    {
        return rule_auto();
    }

    else return ERR_SYNTAX;
}
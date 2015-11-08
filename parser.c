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

	while ((returnValue == ERR_None))			//no errors so far
	{
		scanner(srcFile);						//get next token
		printf("Token: %s \n", token.area);
		if (token.type == EOF)
			break;

		//printf("token: %s 	typ: %d \n", token.area, token.type);

		returnValue = rule_funcdef(srcFile);		//start parsing recursively
	}

    return returnValue;
}

// rule:	<prog> -> type id <param-list> <func-defined> 
int rule_funcdef()
{
	if ((token.type =! KEY_WORD) || ( (strcmp(token.area, "int") != 0) && (strcmp(token.area, "double") != 0) && (strcmp(token.area, "string") != 0)) )
		return ERR_SYNTAX;

	//vytvorenie polozky TS pre funkciu
printf("koala\n");
	scanner(srcFile);
	printf("Token: %s \n", token.area);
	if (token.type != IDENTIFIER)
		return ERR_SYNTAX;

	//pridanie ID funkcie
printf("cajka\n");
	int error = rule_paramList();
	if (error != ERR_None)
		return error;

printf("avokado\n");

	error = rule_funcDefined();
	if (error != ERR_None)
		return error;


	return ERR_None;
}

//rule:		<func-defined> -> { <st-list> <prog>		||		; <prog>
int rule_funcDefined()
{
	scanner(srcFile);
	printf("Token_fDefined: %s \n", token.area);

	if (strcmp(token.area, ";") == 0)
		return ERR_None;

	if (strcmp(token.area, "{") != 0)
		return ERR_SYNTAX;

	int error = rule_stList();
	if (error != ERR_None)
		return error;

	return ERR_None;
}

//rule:		<param-list> -> ( <param> <param-next>		|		()
int rule_paramList()
{
	scanner(srcFile);

	printf("Token: %s \n", token.area);
	if (strcmp(token.area, "(") != 0)
		return ERR_SYNTAX;

	scanner(srcFile);
	printf("Token: %s \n", token.area);

	if (strcmp(token.area, ")") == 0)
		return ERR_None;

	int error = rule_param();
	if (error != ERR_None)
		return error;

	error = rule_paramNext();
	if (error != ERR_None)
		return error;

	return ERR_None;
}

//rule:		<param> -> type id
int rule_param()	
{
	if ((token.type =! KEY_WORD) || ( (strcmp(token.area, "int") != 0) && (strcmp(token.area, "double") != 0) && (strcmp(token.area, "string") != 0)))
		return ERR_SYNTAX;

	// ulozenie typu parametra do tabulky

	scanner(srcFile);
	printf("Token: %s \n", token.area);
	if (token.type != IDENTIFIER)
		return ERR_SYNTAX;

	//pridanie ID premennej do tabulky

	return ERR_None;
}

//rule:		<param-next> -> , <param> <param-next>		|	)
int rule_paramNext()
{
	scanner(srcFile);
	printf("Token paramnext: **%s** \n", token.area);
	
	if (strcmp(token.area, ",") == 0)
	{
		printf("Som tu!\n");

		scanner(srcFile);
		printf("Token paramnext next: **%s** \n", token.area);
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

		printf("BAD WOLF\n");
		if (strcmp(token.area, ")") == 0)
			return ERR_None;
	}
	
	printf("paramnext zla opica\n");
	return ERR_SYNTAX;
}	

//rule:		<st-list> -> <statement> ; <st-list> 	| 	}
int rule_stList()
{
	scanner(srcFile);
	printf("Token: %s \n", token.area);

	if (strcmp(token.area, "}") == 0)
		return ERR_None;

	int error = rule_statement();
		if (error != ERR_None)
			return error;

	scanner(srcFile);
	printf("Token: %s \n", token.area);

	if (strcmp(token.area, ";") != 0)
		return ERR_SYNTAX;

	error = rule_stList();
		if (error != ERR_None)
			return error;

	return ERR_None;
}

//rule:		TODO
int rule_statement()
{
	//scanner(srcFile);
	//printf("Token (nie je odignorovany because reasons): %s \n", token.area);

	if (strcmp(token.area, "pomocna") != 0)
		return ERR_SYNTAX;

	return ERR_None;
}		
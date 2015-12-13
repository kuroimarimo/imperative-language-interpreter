#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lex_an.h"
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
 *  Vojtěch Václavík	(xvacla22)
 *  Petr Vančo			(xvanco05)
 *  Filip Vaško         (xvasko10)
 *  Dominik Vašek		(xvasek06)
 *  Valentína Straková	(xstrak27)
 */

#include "error.h"
#include "parser.h"
#include "garbage_collector.h"

int main(int argc, char** argv)
{
	if (argc != 2)
    {
		printErrorDetails(ERR_InvalidCommndLineArgs);
		return getReturnValue(ERR_InvalidCommndLineArgs);

        
    }
    if ((source = fopen(argv[1], "r")) == NULL)
    {
		printErrorDetails(ERR_CannotOpenFile);
        return getReturnValue(ERR_CannotOpenFile);
    }
	garbageListInit(&garbageList);
	parse();               // parse the code

    // cleanup
	fclose(source);
	garbageCleanup(&garbageList);

    return 0;
}

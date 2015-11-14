#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lex_an.h"
#include "error.h"
#include "parser.h"



int main(int argc, char** argv)
{
    FILE *source;
    SError errorState;

    if (argc != 2)
    {
        printf("I can't find any file! \n");
        return 1;
    }
    if ((source = fopen(argv[1], "r")) == NULL)
    {
        printf("I coudn't open the file! \n");
        return 1;
    }

/*
    bool test = true;

    while (test) {
        scanner(source);
        if (token.type == EOF)
            test = false;
        else if (token.unie.area != NULL)
            printf("Tok: %s %d\n", token.unie.area, token.type);
    }
*/
    errorState.state = parse(source);               // parse the code
    errorState.line = token.counter_of_lines;       // this stuff doesn't work the way I'd like it to work

    if (errorState.state != ERR_None)
        printErrorDetails(errorState);
    else
        printf("Syntax OK.\n");

    fclose(source);

    return getReturnValue(errorState.state);
}

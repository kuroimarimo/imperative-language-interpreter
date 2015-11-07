#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lex_an.h"



int main(int argc, char** argv)
{
    FILE *source;
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


    bool test = true;

    while (test) {
        scanner(source);
        if (token.type == EOF)
            test = false;
        else
            printf("Tok: %s \n", token.area);
    }

    fclose(source);

    return 0;
}

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lex_an.h"



int main(int argc, char** argv)
{
    FILE *soubor;
    if (argc != 2)
    {
        printf("Neni zadan vstupni soubor! \n");
        return 1;
    }
    if ((soubor = fopen(argv[1], "r")) == NULL)
    {
        printf("Soubor se nepodarilo otevrit! \n");
        return 1;
    }


    bool test = true;

    while (test) {
        scanner(soubor);
        if (token.typ == EOF)
            test = false;

        printf("Tok: %s \n", token.obsah);
    }

    fclose(soubor);

    return 0;
}

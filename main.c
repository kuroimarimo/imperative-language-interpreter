#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lex_an.h"

#define POCET_KLIC_SLOV 10


const char *klicova_slova [POCET_KLIC_SLOV] = {
        "auto", "cin", "cout", "double", "else", "for", "if", "int", "return", "string"
};

tToken token;

int initToken () {
    token.obsah = (char *) malloc(2);
    if (token.obsah == NULL)
        return -1;
        /// chyba malloc
    token.stav = TOK_PRAZDNY;

    return 0;
}

int naplnToken (char znak) {

        /// prvni inicializace
    if (token.pocitadlo == 0) {
        token.obsah = (char *) malloc(2);
        if (token.obsah == NULL)
            return -1;
    }
    else {      /// navyseni kapacity
        token.obsah = (char *) realloc(token.obsah, token.pocitadlo + 1);
        if (token.obsah == NULL)
            return -1;
            /// chyba realloc
    }

    token.obsah [token.pocitadlo] = znak;
    token.obsah [token.pocitadlo + 1] = '\0';
    (token.pocitadlo)++;

    return 0;
}

void uvolniToken () {
    free(token.obsah);
    token.pocitadlo = 0;
}



int main(int argc, char** argv)
{
    FILE *soubor;
    if (argc == 1)
    {
        printf("Neni zadan vstupni soubor! \n");
        return 1;
    }
    if ((soubor = fopen(argv[1], "r")) == NULL)
    {
        printf("Soubor se nepodarilo otevrit! \n");
        return 1;
    }

    initToken();

    int c = 0;
    int hodnota = POCATEK;
    bool test = true;
    while (test && ( c = fgetc(soubor)) != EOF)
    {
        switch (hodnota) {
        case POCATEK:
                /** Rozpoznani znaku */
            if isalpha(c)
                hodnota = PISMENO;
            else if isdigit(c)
                hodnota = CISLO;
            else if (c == ';')
                hodnota = STREDNIK;
            else if (c == '_')
                hodnota = PODTRZITKO;
            else if (c == ' ')
                hodnota = MEZERA;
            else if (c == '\t')
                hodnota = TABULATOR;
            else if (c == '(')
                hodnota = L_ZAVORKA;
            else if (c == ')')
                hodnota = P_ZAVORKA;
            else if (c == ']')
                hodnota = L_HRAN_ZAV;
            else if (c == '[')
                hodnota = P_HRAN_ZAV;
            else if (c == '{')
                hodnota = L_SLOZ_ZAV;
            else if (c == '}')
                hodnota = P_SLOZ_ZAV;
            else if (c == '+')
                hodnota = PLUS;
            else if (c == '-')
                hodnota = MINUS;
            else if (c == '*')
                hodnota = KRAT;
            else if (c == '/')
                hodnota = DELENO;
            else if (c == '%')
                hodnota = MODULO;
            else if (c == '!')
                hodnota = VYKRICNIK;
            else if (c == '?')
                hodnota = OTAZNIK;
            else if (c == '"')
                hodnota = RETEZEC;
            else if (c == ',')
                hodnota = CARKA;
         /*   else if (c == '.')
                hodnota = TECKA;    */
            else if (c == '<')
                hodnota = ROZ_MENSI;
            else if (c == '>')
                hodnota = ROZ_VETSI;
            else if (c == '=')
                hodnota = ROVNITKO;
            else
                hodnota = JINY_ZNAK;
            printf("Pocatek %c %d \n", c, hodnota);

            ungetc(c, soubor);
        break;

        case PISMENO:
        case PODTRZITKO:
            if (isalpha(c) || c == '_' || (isdigit(c) && (token.pocitadlo >= 1))) {
                naplnToken(c);
                printf("%c", c); }
            else {
                /// ukonci token a posli ho
                hodnota = POCATEK;
            }
        break;

        case CISLO:
            printf("Precetl cislo:\n");

        break;


        }

    }





    printf("Token %s\n", token.obsah);

    printf("\n");
    fclose(soubor);

    return 0;
}

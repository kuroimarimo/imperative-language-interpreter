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
    token.obsah = NULL;
    token.pocitadlo = 0;
    token.typ = TOK_NULL;

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
    initToken();
}



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
            else if (c == ' ')          // preskocim znak
                break;
            else if (c == '\t')
                break;                  // preskocim znak
            else if (c == '\n')
                break;                  // preskocim znak
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
            else if (c == '.')
                return -1; ///hodnota = TECKA;    /// Error - cislo nemuze zacinat teckou
            else if (c == '<')
                hodnota = MENSI;
            else if (c == '>')
                hodnota = VETSI;
            else if (c == '=')
                hodnota = ROVNITKO;
            else
                hodnota = JINY_ZNAK;
            printf("Pocatek %c %d \n", c, hodnota);

            naplnToken(c);
            break;

        case PISMENO:       /// identifikator, zacina pismenem nebo _
        case PODTRZITKO:

            if (isalpha(c) || c == '_' || isdigit(c) )
                naplnToken(c);
            else {
                token.typ = IDENTIFIKATOR;
                printf("Token:%s\n", token.obsah);

                    /// vynuluj token
                    uvolniToken();

                ungetc(c, soubor);
                hodnota = POCATEK;   /// neměl by zde být stav konec ?? ? ??????? 
            }
            break;

        case CISLO:
            if (isdigit(c))
                naplnToken(c);
            else if (c == '.') {    /// desetinne cislo -> double
                hodnota = DES_CISLO;
                naplnToken(c);
            }
            else if (c == 'e' || c == 'E') {
                hodnota = EXP_CISLO;
                naplnToken(c);
            }
            else {
                token.typ = INT;
                printf("Token:%s\n", token.obsah);

                    /// vynuluj token
                    uvolniToken();

                ungetc(c, soubor);
                hodnota = POCATEK;
            }
            break;

        case DES_CISLO:
            if (isdigit(c))
                naplnToken(c);
            else if (c == '.')
                return -1;      /// Lex_an chyba - zadana druha deseinna tecka
            else {
                token.typ = DOUBLE;
                printf("Token:%s\n", token.obsah);

                    /// vynuluj token
                    uvolniToken();

                ungetc(c, soubor);
                hodnota = POCATEK;
            }
            break;

        case EXP_CISLO:
            if (isdigit(c))
                naplnToken(c);
            else if (c == '+' || c == '-') {
                naplnToken(c);
                hodnota = EXP_CISLO_ZN;
            }
            else if (c == 'e' || c == 'E')
                return -1;      /// Lex_an chyba - zadane druhe E
            else {
                token.typ = DOUBLE;
                printf("Token:%s\n", token.obsah);

                    /// vynuluj token
                    uvolniToken();

                ungetc(c, soubor);
                hodnota = POCATEK;
            }
            break;
        








        case RETEZEC:
            if (c != '"')
                naplnToken(c);
            else {
                naplnToken(c);
                token.typ = RETEZEC;
                printf("Token:%s\n", token.obsah);

                    /// vynuluj token
                    uvolniToken();

                /// ungetc(c, soubor); /// vytvari nekonecny cyklus
                hodnota = POCATEK;
                }
            break;

     
    case MENSI:
            if (c == '<')
            {
                hodnota= ROZ_MENSI;
                naplnToken(c);
                

            }
            else
            {
                ungetc(c,soubor);
                test=false;
            }
            break;

   case VETSI:
            if (c == '>')
            {
                hodnota= ROZ_VETSI;
                naplnToken(c);
                

            }
            else
            {
                ungetc(c,soubor);
                test=false;
            }
            break;


    case ROVNITKO:
            if(c == '<')
            {
                naplnToken(c);
                hodnota=ROVNITKOMENSI;

            }
            else if (c== '>')
            {
                naplnToken(c);
                hodnota=ROVNITKOVETSI;
            }
            else 
            {
                ungetc(c,soubor);
                test=false; 
            }  


    case VYKRICNIK:
            if (c == '=')
            {
                naplnToken(c);
                hodnota=NEGACE;

            }
    
    case STREDNIK: // koncovy stav 
    case L_ZAVORKA:
    case P_ZAVORKA:
    case L_HRAN_ZAV:    
    case P_HRAN_ZAV:     
    case L_SLOZ_ZAV:     
    case P_SLOZ_ZAV:
    case PLUS:
    case MINUS:
    case KRAT:
    case DELENO:
    case MODULO:
    case NEGACE:
    case CARKA:
    case ROVNITKOMENSI:
    case ROVNITKOVETSI:
    
        naplnToken(c);
        test=false;
        break;
        

        }

    }


    printf("\n");
    fclose(soubor);

    return 0;
}

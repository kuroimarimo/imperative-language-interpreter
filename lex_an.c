#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lex_an.h"

const char *klicova_slova [POCET_KLIC_SLOV] = {
        "auto", "cin", "cout", "double", "else", "for", "if", "int", "return", "string"
};

void initToken () {
    if (token.obsah != NULL)
        free(token.obsah);
    token.pocitadlo = 0;
    token.typ = TOK_NULL;

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

int scanner (FILE *soubor) {
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
            else if (c == ' ')          // preskocim znak mezery
                break;
            else if (c == '\t')
                break;                  // preskocim znak tabulatoru
            else if (c == '\n')
                break;                  // preskocim znak konce radku
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
            else if (c == '/') {
                hodnota = DELENO;
                break;
                }
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
                return -1;          ///hodnota = TECKA;    /// Error - cislo nemuze zacinat teckou
            else if (c == '<')
                hodnota = MENSI;
            else if (c == '>')
                hodnota = VETSI;
            else if (c == '=')
                hodnota = ROVNITKO;
            else
                hodnota = JINY_ZNAK;

            if (hodnota != RETEZEC) /// neukladam uvozovky na zacatku retezce
                naplnToken(c);
            break;


       case PISMENO:       /// identifikator, zacina pismenem nebo '_' ; dalsi znaky mohou byt cisla
        case PODTRZITKO:

            if (isalpha(c) || c == '_' || isdigit(c) )
                naplnToken(c);
            else {
                token.typ = IDENTIFIKATOR;
                ungetc(c, soubor);
                test = false;
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
                ungetc(c, soubor);
                test = false;

            }
            break;

        case DES_CISLO:
            if (isdigit(c))
                naplnToken(c);
            else if (c == '.') /* || (c == 'e' || c == 'E'))    */
                return -1;      /// Lex_an chyba - zadana druha deseinna tecka
            else {
                token.typ = DOUBLE;
                ungetc(c, soubor);
                test = false;
            }
            break;

        case EXP_CISLO:
            if (isdigit(c)) {
                naplnToken(c);
                hodnota = EXP_CIS_KONEC;    /// za E jsou cifry
            }
            else if (c == '+' || c == '-') {
                naplnToken(c);
                hodnota = EXP_CISLO_ZN;     /// znamenkova mocnina exp. cisla: 1E-12
            }
            else                /// za E nejsou cifry
                return -1;

            break;

        case EXP_CISLO_ZN:
            if (isdigit(c)) {
                naplnToken(c);
                hodnota = EXP_CIS_KONEC;    /// za E jsou cifry
                }
            else
                return -1;

            break;

        case EXP_CIS_KONEC:
            if (isdigit(c))
                naplnToken(c);
            else {
                token.typ = DOUBLE;
                ungetc(c, soubor);
                test = false;
            }
            break;

        case RETEZEC:
            if (c != '"')
                naplnToken(c);
            else {
                token.typ = RETEZEC;
                /// ungetc(c, soubor); - vytvari nekonecny cyklus
                test = false;
                }
            break;


    case MENSI:
            if (c == '<')
            {
                hodnota = ROZ_MENSI;
                naplnToken(c);


            }
            else
            {
                ungetc(c,soubor);
                test = false;
            }
            break;

   case VETSI:
            if (c == '>')
            {
                hodnota = ROZ_VETSI;
                naplnToken(c);


            }
            else
            {
                ungetc(c,soubor);
                test=false;
            }
            break;

    case DELENO:
        if (c != '/' && c!= '*') {
            ungetc(c,soubor);
            test=false;
        }
        else if (c == '/')
            hodnota = RAD_KOMENTAR;
        else
            hodnota = KOMENTAR;
        break;

    case RAD_KOMENTAR:
        if (c == '\n') {
            hodnota = POCATEK;
        }


    case ROVNITKO:
            if(c == '<')
            {
                naplnToken(c);
                hodnota=ROVNITKOMENSI;  /// není to blbost? nemá být <= a >=

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

    if (c == EOF)
        token.typ = EOF;

    return 0;
}

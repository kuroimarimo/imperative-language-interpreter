#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lex_an.h"

const char *klicova_slova [POCET_KLIC_SLOV] = {
    "auto", "cin", "cout", "double", "else", "for", "if", "int", "return", "string"
};

const char *vestav_funkce [POC_VEST_FUNKCI] = {
    "length", "substr", "concat", "find", "sort"
};

void initToken () {
    if (token.obsah != NULL)
        free(token.obsah);
    token.pocitadlo = 0;
    token.typ = TOK_NULL;

}

int naplnToken (char znak) {
    int zvetseni_tok = 0;

        /// prvni inicializace
    if (token.pocitadlo == 0) {
        token.obsah = (char *) malloc(3);   /// 2 znaky + \O
        if (token.obsah == NULL)
            return -1;
    }
    else if ((token.pocitadlo) % 10 == 0) {      /// navyseni kapacity na 10 nebo o + 10
        if (token.pocitadlo < 3)
            zvetseni_tok = 10;
        else
            zvetseni_tok = token.pocitadlo + 10;

        token.obsah = (char *) realloc(token.obsah, zvetseni_tok + 1);
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
                naplnToken(c);
                hodnota = C_IN;
            }
            else if (c == '=')
            {
                naplnToken(c);
                hodnota = MENSI_ROVNO;
            }
            else
            {
                hodnota = MENSI;
                ungetc(c,soubor);
            }
            test = false;
            break;

   case VETSI:
            if (c == '>')
            {
                naplnToken(c);
                hodnota = C_OUT;
            }
            else if (c == '=')
            {
                naplnToken(c);
                hodnota = VETSI_ROVNO;
            }
            else
            {
                hodnota = VETSI;
                ungetc(c,soubor);

            }
            test = false;
            break;

    case DELENO:
        if (c != '/' && c!= '*') {
            naplnToken('/');

            ungetc(c,soubor);
            test = false;
        }
        else if (c == '/')
            hodnota = RAD_KOMENTAR;
        else
            hodnota = KOMENTAR;
        break;

    case RAD_KOMENTAR:
        if (c == '\n')
            hodnota = POCATEK;

        break;

    case KOMENTAR:
        if (c == '*')
            hodnota = KOMENTAR_KON;
        break;

    case KOMENTAR_KON:
        if (c == '/')
            hodnota = POCATEK;
        else
            hodnota = KOMENTAR;
        break;



    case ROVNITKO:
                ungetc(c,soubor);
                test=false;
        break;


    case VYKRICNIK:
            if (c == '=')
            {
                naplnToken(c);
                hodnota = NEGACE;
            }
        break;

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

        naplnToken(c);
        test=false;
        break;


        }

    }

    for (int i = 0; i < POCET_KLIC_SLOV; i++) {
        int test_for;
        test_for = strcmp(token.obsah, klicova_slova[i]);

        if (test_for == 0) {
            // printf("Klic_slovo: %s\n", token.obsah);
            token.typ = KLICOVE_SLOVO;
            i = 10;
        }
    }

    for (int i = 0; i < POC_VEST_FUNKCI; i++) {
        int test_for;
        test_for = strcmp(token.obsah, vestav_funkce[i]);

        if (test_for == 0) {
            // printf("vest fce: %s\n", token.obsah);
            token.typ = VEST_FUNKCE;
            i = 10;
        }
    }

    if (c == EOF)
        token.typ = EOF;

    return 0;
}

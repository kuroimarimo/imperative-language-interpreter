#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lex_an.h"

const char *key_words [COUNT_OF_KEY_WORDS] = {
    "auto", "cin", "cout", "double", "else", "for", "if", "int", "return", "string"
};

const char *built_in_functions [COUNT_OF_BUILT_IN_FUNCTIONS] = {
    "length", "substr", "concat", "find", "sort"
};



void initToken () {  // inicializovat token
    if (token.area != NULL)
        free(token.area);
    token.area = NULL;
    token.counter = 0;
    token.type = TOK_NULL;
    token.counter_of_lines=0;

}

int fillToken (char character) {  // naplnit token
    // int extension_tok = 0;

        /// prvni inicializace
    if (token.counter == 0) {
        token.area = (char *) malloc(3);   /// 2 charactery + \O
        token.sizeof_area = 2;
        if (token.area == NULL)
            return -1;
    }
    else if (token.counter == token.sizeof_area) {      /// navyseni kapacity o dvojnasobek
        token.sizeof_area = token.sizeof_area * 2;

        token.area = (char *) realloc(token.area, (token.sizeof_area + 1));
        if (token.area == NULL)
            return -1;
            /// chyba realloc
    }

    token.area [token.counter] = character;
    token.area [token.counter + 1] = '\0';
    (token.counter)++;

    return 0;
}

int scanner (FILE *source) {
    initToken();
    token.unie.int_number = 5;

    int c = 0;
    int value = START;
    bool test = true;
    while (test && ( c = fgetc(source)) != EOF)
    {
        switch (value) {
        case START:
                /** Rozpoznani characteru */
            if isalpha(c)
                value = LETTER;
            else if (c == '0') {
                value = NUMBER_START_ZERO;
                break;
                }
            else if isdigit(c)
                value = NUMBER;
            else if (c == '_')
                value = UNDERSCORE;
            else if (c == ' ')          // preskocim character mezery
                break;
            else if (c == '\t')
                break;                  // preskocim character tabulatoru
            else if (c == '\n') {
                token.counter_of_lines++;
                break;                  // preskocim character konce radku
                }
            else if (c == '/') {
                value = DIVIDE;
                break;
                }
            else if (c == '!')
                value = EXCLAMATION_MARK;  //vykřičník
            else if (c == '?')
                value = QUESTION_MARK;
            else if (c == '"')
                value = STRING;

            else if (c == '.')
                return -1;          ///value = TECKA;    /// Error - cislo nemuze zacinat teckou
            else if (c == '<')
                value = LESS;
            else if (c == '>')
                value = GREATER;
            else if (c == '=')
                value = ASSIGNMENT;
            else {              /// jednoznakove tokeny
                test = false;

                if (c == ';')
                    token.type = SEMICOLON;
                else if (c == ',')
                    token.type = COMMA;
                else if (c == '(')
                    token.type = L_BRACKET;
                else if (c == ')')
                    token.type = R_BRACKET;
                else if (c == ']')
                    token.type = L_SQUARE_BRACKET;
                else if (c == '[')
                    token.type = R_SQUARE_BRACKET;
                else if (c == '{')
                    token.type = L_CURLY_BRACKET;
                else if (c == '}')
                    token.type = R_CURLY_BRACKET;
                else if (c == '+')
                    token.type = PLUS;
                else if (c == '-')
                    token.type = MINUS;
                else if (c == '*')
                    token.type = MULTIPLY;
                else if (c == '%')
                    token.type = MODULO;
                else
                    token.type = ANOTHER_CHAR;  /// má smysl?

                /// break;
            }



            if (value != STRING) /// neukladam uvozovky na zacatku retezce
                fillToken(c);
            break;


       case LETTER:       /// identifikator, zacina pismenem nebo '_' ; dalsi charactery mohou byt cisla
       case UNDERSCORE:

            if (isalpha(c) || c == '_' || isdigit(c) )
                fillToken(c);
            else {
                token.type = IDENTIFIER;
                ungetc(c, source);
                test = false;
            }
            break;

        case NUMBER_START_ZERO:
            if (c == '0')           /// ignoruj nulu
                break;
            else if ((isdigit(c))  ) {  /// 1 .. 9

            }
            else if ((c == '.') || (c == 'E') || (c == 'e') ) {
                fillToken('0');
            }
            else {
                test = false;
                fillToken('0');
                token.type = INT;
            }

            ungetc(c, source);
            value = NUMBER;

            break;

        case NUMBER:

            if (isdigit(c))
                fillToken(c);
            else if (c == '.') {    /// desetinne cislo -> double
                value = DEC_NUMBER;
                fillToken(c);
            }
            else if (c == 'e' || c == 'E') {
                value = EXP_NUMBER;
                fillToken(c);
            }
            else {
                token.type = INT;
                ungetc(c, source);
                test = false;

            }
            break;

        case DEC_NUMBER:
            if (isdigit(c)) {
                fillToken(c);
                value = DEC_NUMBER_END;
            }
            else if (c == '.')  /** || (c == 'e' || c == 'E'))    */
                return -1;      /// Lex_an chyba - zadana druha desetinna tecka
            else {
                token.type = DOUBLE;
                ungetc(c, source);
                test = false;
            }
            break;

        case DEC_NUMBER_END:
            if (isdigit(c))
                fillToken(c);  /// Lex_an chyba - zadana druha desetinna tecka
            else if (c == '.')
                return -1;
            else if ((c == 'e' || c == 'E')) {
                fillToken(c);
                value = EXP_NUMBER;
            }
            else {
                token.type = DOUBLE;
                ungetc(c, source);
                test = false;
            }

            break;

        case EXP_NUMBER:
            if (isdigit(c)) {
                fillToken(c);
                value = EXP_NUMBER_END;    /// za E jsou cifry
            }
            else if (c == '+' || c == '-') {
                fillToken(c);
                value = EXP_NUMBER_SING;     /// znamenkova mocnina exp. cisla: 1E-12
            }
            else                /// za E nejsou cifry
                return -1;

            break;

        case EXP_NUMBER_SING:
            if (isdigit(c)) {
                fillToken(c);
                value = EXP_NUMBER_END;    /// za E jsou cifry
                }
            else
                return -1;

            break;

        case EXP_NUMBER_END:
            if (isdigit(c))
                fillToken(c);
            else {
                token.type = DOUBLE;
                ungetc(c, source);
                test = false;
            }
            break;

        case STRING:
            if (c != '"')
                fillToken(c);
            else {
                token.type = STRING;
                /// ungetc(c, source); - vytvari nekonecny cyklus
                test = false;
                }
            break;


    case LESS:
            if (c == '<')
            {
                fillToken(c);
                value = C_IN;
            }
            else if (c == '=')
            {
                fillToken(c);
                value = LESS_ROVNO;
            }
            else
            {
                value = LESS;
                ungetc(c,source);
            }
            test = false;
            break;

   case GREATER:
            if (c == '>')
            {
                fillToken(c);
                value = C_OUT;
            }
            else if (c == '=')
            {
                fillToken(c);
                value = GREATER_ROVNO;
            }
            else
            {
                value = GREATER;
                ungetc(c,source);

            }
            test = false;
            break;

    case DIVIDE:
        if (c != '/' && c!= '*') {
            fillToken('/');

            ungetc(c,source);
            test = false;
        }
        else if (c == '/')
            value = LINE_COMMENT;
        else
            value = COMMENT;
        break;

    case LINE_COMMENT:
        if (c == '\n')
            value = START;

        break;

    case COMMENT:
        if (c == '*')
            value = COMMENT_KON;
        break;

    case COMMENT_KON:
        if (c == '/')
            value = START;
        else
            value = COMMENT;
        break;



    case ASSIGNMENT:
            if (c == '=') {
                fillToken(c);
                token.type = EQUAL;
            }
            else {
                ungetc(c,source);
                token.type = ASSIGNMENT;
            }

            test=false;
        break;


    case EXCLAMATION_MARK:
            if (c == '=')
            {
                fillToken(c);
                token.type = NEGACE;
            }
            else {
                ungetc(c, source);
            }
                test = false;

        break;
/*
    case SEMICOLON: // koncovy stav
    case L_BRACKET:
    case R_BRACKET:
    case L_SQUARE_BRACKET:
    case R_SQUARE_BRACKET:
    case L_CURLY_BRACKET:
    case R_CURLY_BRACKET:
    case PLUS:
    case MINUS:
    case MULTIPLY:
    case MODULO:
    case COMMA:

    case NEGACE:

        //  fillToken(c);
        ungetc(c, source);
        test=false;
        break;
*/

        }

    }
    if ((token.type == IDENTIFIER) && (token.area != NULL)) {
        for (int i = 0; i < COUNT_OF_KEY_WORDS; i++) {
            int test_for;
            test_for = strcmp(token.area, key_words[i]);

            if (test_for == 0) {
                token.type = KEY_WORD;
                i = 10;     /// ukončím for smyčku
            }
        }

        for (int i = 0; i < COUNT_OF_BUILT_IN_FUNCTIONS; i++) {
            int test_for;
            test_for = strcmp(token.area, built_in_functions[i]);

            if (test_for == 0) {
                token.type = BUILT_IN_FUNCTION;
                i = 10;     /// ukončím for smyčku
            }
        }
    }

    if (c == EOF)
        token.type = EOF;

    return 0;
}

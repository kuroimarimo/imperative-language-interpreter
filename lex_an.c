#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>

#include "error.h"
#include "lex_an.h"


int tokenCopy (tToken *dst, tToken src)
{
    if (dst->area != NULL)
        free(dst->area);
    if (src.area != NULL)
    {
        if ((dst->area = malloc((strlen(src.area) + 1) * sizeof(char))) == NULL)
            return ERR_AllocFailed;

        strcpy(dst->area, src.area);
    }

    dst->int_numb = src.int_numb;
    dst->double_numb = src.double_numb;
    dst->type = src.type;
   // dst->expression = src.expression;  je stále potřeba ... případvně vymazat z tokenu
    dst->counter_of_lines = src.counter_of_lines;
    dst->counter = src.counter;
    dst->sizeof_area = src.sizeof_area;

    return ERR_None;
}

void tokenSwap()
{
    tToken temp;

    /*tokenCopy(&temp, token);
    tokenCopy(&token, oldToken);
    tokenCopy(&oldToken, temp);*/

    temp = token;
    token = oldToken;
    oldToken = temp;

    return;
}

void ungetToken()
{
    tokenSwap();
    ungotToken = 1;

    return;
}

void initToken () {  // inicializovat token
    if (token.area != NULL)
        free(token.area);
    token.area = NULL;
    token.counter = 0;
    token.type = TOK_NULL;

    static int tmp = 0;
    if(!tmp){
        token.counter_of_lines=1;
        tmp++;
    }


}

void fillToken (char character) {  // naplnit token
    // int extension_tok = 0;

        /// prvni inicializace
    if (token.counter == 0) {
        token.area = (char *) malloc(3);   /// 2 charactery + \O
        token.sizeof_area = 2;
        if (token.area == NULL){
            errorState.state= ERR_AllocFailed;
            errorState.line=token.counter_of_lines;
            fatalError (errorState);
            }

    }
    else if (token.counter == token.sizeof_area) {      /// navyseni kapacity o dvojnasobek
        token.sizeof_area = token.sizeof_area * 2;

        token.area = (char *) realloc(token.area, (token.sizeof_area + 1));
        if (token.area == NULL){
            errorState.state= ERR_AllocFailed;
            errorState.line=token.counter_of_lines;
            fatalError (errorState);
            }
            /// chyba realloc
    }

    token.area [token.counter] = character;
    token.area [token.counter + 1] = '\0';
    (token.counter)++;

    return ;
}

int scanner (FILE *source) {

    if (ungotToken)             //ungetToken was called
    {
        tokenSwap();
        ungotToken = 0;
        return 0;
    }

    tokenCopy(&oldToken, token);

    initToken();

    int c = 0;
    int counter = 0;
    char array_x [3];
    int value = START;
    bool test = true;

    while (test && ( c = fgetc(source)) != EOF)
    {
        switch (value) {
        case START:
                /* Rozpoznani characteru */

            if (isalpha(c)){
                value = LETTER;
                break;
                }
            else if (c == '0') {
                value = NUMBER_START_ZERO;
                break;
                }
            else if (isdigit(c)){
                value = NUMBER;
                break;
            }

            switch (c){
            case '_':
                 value = UNDERSCORE;
                 break;
            case ' ':          // preskocim character mezery
                 break;

            case '\t':
                break;                  // preskocim character tabulatoru
            case '\n':
                token.counter_of_lines++;
                break;                  // preskocim character konce radku
                break;
            case '/':
                value = DIVIDE;
                break;
                break;
            case '\\':
                value = BACKSLASH;
                break;
                break;
            case '!':
                value = EXCLAMATION_MARK;  //vykřičník
                break;
            case '?':
                value = QUESTION_MARK;
 	 	 	 	break;
            case '"':
                value = STRING;
                break;
            case '<':
                value = LESS;
 	              break;
            case '>':
                value = GREATER;
 	 	 	    break;
            case '=':
                value = ASSIGNMENT;
 	 	 	 	break;
            case '+':
                value = PLUS;
 	 	 	 	break;
            case '-':
                value = MINUS;
                break;
            default:
                          /// jednoznakove tokeny
               test = false;

                switch (c){
                case ';':
                    token.type = SEMICOLON;
                    break;
                 case ',':
                    token.type = COMMA ;
 	 	 	 	    break;
                case  '(':
                    token.type = L_BRACKET;
 	 	 	 	    break;
                case  ')':
                    token.type = R_BRACKET;
 	 	 	 	     break;
                case  ']':
                    token.type = L_SQUARE_BRACKET;
 	 	 	 	    break;
                case  '[':
                    token.type = R_SQUARE_BRACKET;
 	 	 	 	    break;
                case  '{':
                    token.type = L_CURLY_BRACKET;
 	 	 	 	     break;
                case  '}':
                    token.type = R_CURLY_BRACKET;
 	 	 	 	    break;
                case  '*':
                    token.type = MULTIPLY;
 	 	 	 	     break;
                case  '%':
                    token.type = MODULO;
 	 	 	      	 break;
                default:

                        errorState.state=ERR_UnknownChar;
                        errorState.line=token.counter_of_lines;
                        fatalError (errorState);

                }

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
            else if (isalpha(c) || c == '_')
            {
                errorState.state=ERR_LEXICAL;
                errorState.line=token.counter_of_lines;
                fatalError (errorState);
            }
            else {
                test = false;
                fillToken('0');
                token.type = INT_NUMBER;
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
            else if (isalpha(c) || c == '_')
            {
                errorState.state=ERR_LEXICAL;
                errorState.line=token.counter_of_lines;
                fatalError (errorState);

            }
            else {
                token.type = INT_NUMBER;
                ungetc(c, source);
                test = false;

            }
            break;

        case DEC_NUMBER:    /// 123. -> musi prijit cislo, jinak chyba
            if (isdigit(c)) {
                fillToken(c);
                value = DEC_NUMBER_END;
            }
            else /* if (c == '.')   || (c == 'e' || c == 'E'))    */
                {
                        errorState.state=ERR_NumberShape;
                        errorState.line=token.counter_of_lines;
                        fatalError (errorState);
                    }       /// Lex_an chyba - zadana druha desetinna tecka
            /*else {
                token.type = DOUBLE_NUMBER;
                ungetc(c, source);
                test = false;
            }*/
            break;

        case DEC_NUMBER_END:
            if (isdigit(c))
                fillToken(c);
            else if ((c == 'e' || c == 'E'))
            {
                fillToken(c);
                value = EXP_NUMBER;
            }
            else if (c == '.' || c == '_' || isalpha(c))  /// Lex_an chyba - zadana druha desetinna tecka
            {
                errorState.state=ERR_LEXICAL;
                errorState.line=token.counter_of_lines;
                fatalError (errorState);
            }
            else {
                token.type = DOUBLE_NUMBER;
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
                value = EXP_NUMBER_SIGN;     /// znamenkova mocnina exp. cisla: 1E-12
            }
            else                /// za E nejsou cifry
                {
                    errorState.state=ERR_NumberShape;
                    errorState.line=token.counter_of_lines;
                    fatalError (errorState);
                }

            break;

        case EXP_NUMBER_SIGN:
            if (isdigit(c)) {
                fillToken(c);
                value = EXP_NUMBER_END;    /// za E jsou cifry
                }
            else
               {
                    errorState.state=ERR_NumberShape;
                    errorState.line=token.counter_of_lines;
                    fatalError (errorState);
                }

            break;

        case EXP_NUMBER_END:
            if (isdigit(c))
                fillToken(c);
            else if (isalpha(c) || c == '_' || c == '.')
            {
                errorState.state=ERR_LEXICAL;
                errorState.line=token.counter_of_lines;
                fatalError (errorState);
            }
            else {
                token.type = DOUBLE_NUMBER;
                ungetc(c, source);
                test = false;
            }
            break;

        case STRING:
            if (c == '\\')
                value = STRING_ESCAPE;
            else if (c != '"')
                fillToken(c);

            else {
                token.type = STRING;
                /// ungetc(c, source); - vytvari nekonecny cyklus
                test = false;
                }
            break;

        case STRING_ESCAPE:
            value = STRING;

            if (c == 'n')
                fillToken('\n');
            else if (c == 't')
                fillToken('\t');
            else if (c == '\\')
                fillToken('\\');
            else if (c == '"')
                fillToken('\"');
            else if (c == 'x')
                value = STRING_ESCAPE_x1;
            else {
                errorState.state=ERR_StringEscape;
                errorState.line=token.counter_of_lines;
                fatalError (errorState);
            }

            break;

        case STRING_ESCAPE_x1:
            if (isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
            {
                array_x [0] = c;
                array_x [1] = '\0';
                value = STRING_ESCAPE_x2;
            }
            else
                {
                        errorState.state=ERR_StringEscape;
                        errorState.line=token.counter_of_lines;
                        fatalError (errorState);
                    }

            break;

        case STRING_ESCAPE_x2:
            if (isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
            {
                array_x [1] = c;
                array_x [2] = '\0';

                int number = (int) strtol(array_x, NULL, 16);

                if (number >= 1 && number <= 255)
                    fillToken(number + '\0');
                else
                   {
                        errorState.state=ERR_StringEscape;
                        errorState.line=token.counter_of_lines;
                        fatalError (errorState);
                    }

                value = STRING;
            }
            else
               {
                        errorState.state=ERR_StringEscape;
                        errorState.line=token.counter_of_lines;
                        fatalError (errorState);
                    }

            break;

    case LESS:
            if (c == '<')
            {
                fillToken(c);
                token.type = C_OUT;
            }
            else if (c == '=')
            {
                fillToken(c);
                token.type = LESS_EQUAL;
            }
            else
            {
                token.type = LESS;
                ungetc(c,source);
            }
            test = false;

            break;

   case GREATER:
            if (c == '>')
            {
                fillToken(c);
                token.type = C_IN;
            }
            else if (c == '=')
            {
                fillToken(c);
                token.type = GREATER_EQUAL;
            }
            else
            {
                token.type = GREATER;
                ungetc(c,source);

            }
            test = false;

            break;

    case DIVIDE:
        if (c != '/' && c!= '*') {
            fillToken('/');
            token.type = DIVIDE;

            ungetc(c,source);
            test = false;
        }
        else if (c == '/')
            value = LINE_COMMENT;
        else
            value = COMMENT;

        break;

    case BACKSLASH:
        if (c == 'b')
            value = BINARY;
        else if (c == '0')
            value = OCTAL;
        else if (c == 'x')
            value = HEXADECIMAL;
        else {
            errorState.state = ERR_NumberShape;
            errorState.line = token.counter_of_lines;
            fatalError (errorState);
        }

        break;

    case BINARY:
        if (counter == 8 && ((c >= '0' && c <= '9') || isalpha(c))) {
            errorState.state = ERR_NumberShape;
            errorState.line = token.counter_of_lines;
            fatalError (errorState);
        }
        else {
            test = false;
            token.type = BINARY;
            ungetc(c,source);
            break;
        }

        if (c >= '0' && c <= '1') {
            fillToken(c);
            counter++;
        }
        else {
            test = false;
            token.type = BINARY;
            ungetc(c,source);
        }

        break;

    case LINE_COMMENT:
        if (c == '\n'){
           value = START;
           token.counter_of_lines++;
        }
        break;


    case COMMENT:
        if (c == '\n')
            token.counter_of_lines++;
        if (c == '*')
            value = COMMENT_END;

        break;


    case COMMENT_END:
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
                token.type = NEGATION;
            }
            else {
                ungetc(c, source);
            }
                test = false;

        break;


    case PLUS:
        if (c == '+') {
            fillToken(c);
            token.type = INCREMENT;
        }
        else {
            token.type = PLUS;
            ungetc(c, source);
        }

        test = false;

        break;


    case MINUS:
        if (c == '-') {
            fillToken(c);
            token.type = DECREMENT;
        }
        else {
            token.type = MINUS;
            ungetc(c, source);
        }

        test = false;

        break;
        }
    }


    const char *key_words [COUNT_OF_KEY_WORDS] = {
        "auto", "cin", "cout", "double", "else", "for", "if", "int", "return", "string", "while", "do"
    };

    const char *built_in_functions [COUNT_OF_BUILT_IN_FUNCTIONS] = {
        "length", "substr", "concat", "find", "sort"
    };


    if (token.area != NULL)
    {
        if (token.type == IDENTIFIER)
        {
            for (int i = 0; i < COUNT_OF_KEY_WORDS; i++) {
                int test_for;
                test_for = strcmp(token.area, key_words[i]);

                if (test_for == 0) {
                    token.type = K_AUTO + i;
                    i = COUNT_OF_KEY_WORDS;     /// ukončím for smyčku
                }
            }

            for (int i = 0; i < COUNT_OF_BUILT_IN_FUNCTIONS; i++) {
                int test_for;
                test_for = strcmp(token.area, built_in_functions[i]);

                if (test_for == 0) {
                    token.type = B_LENGTH + i;
                    i = COUNT_OF_BUILT_IN_FUNCTIONS;    /// ukončím for smyčku
                }
            }
        }

    long integer;

        if (token.type == INT_NUMBER)
        {
            errno = 0;

            integer = strtol(token.area, NULL, 10);
            /*
            if ((errno == ERANGE && (integer == LONG_MAX)) || (errno != 0 && integer == 0))
            {
                //printf("Warning: Nevejde se do integeru.\n");
            }
            else if (integer > INT_MAX)
            {
                //printf("Warning: Nevejde se do integeru.\n");
            }
            */
            token.int_numb = (int) integer;
        }

        if (token.type == DOUBLE_NUMBER)
        {
            double real_number;
            char *ptr;

            real_number = strtod(token.area, &ptr);
            token.double_numb = real_number;
        }

        if (token.type == BINARY) {
            integer = strtol(token.area, NULL, 2);
            token.int_numb = (int) integer;

            printf("Cislo binarne: %s", token.area);
            printf("Cislo: %ld\n", integer);
        }
    }


    if (c == EOF)
        token.type = EOF;
/* jestli nikdo nepotřebuje token.expression je mo
    if (token.type == INT_NUMBER || token.type== DOUBLE_NUMBER)
        token.expression = IDENTIFIER;
    else
        token.expression = token.type;
*/
    return ERR_None;
}

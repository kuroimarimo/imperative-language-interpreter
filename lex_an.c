#include "lex_an.h"

int tokenCopy (tToken *dst, tToken src)
{

    if (src.area != NULL)
		dst->area = strDuplicate(src.area);

    dst->int_numb = src.int_numb;
    dst->double_numb = src.double_numb;
    dst->type = src.type;
   // dst->expression = src.expression;  je stále potřeba ... případvně vymazat z tokenu
    dst->counter_of_lines = src.counter_of_lines;
    dst->counter = src.counter;
    dst->sizeof_area = src.sizeof_area;

    return ERR_None;
}

void ungetToken()
{
    SWAP(token, oldToken, tToken);
    ungotToken = 1;

    return;
}

void initToken ()       // inicializovat token
{

    token.area = NULL;
    token.counter = 0;
    token.type = TOK_NULL;

    static int tmp = 0;
    if(!tmp)
    {
        token.counter_of_lines = 1;
        tmp++;
    }


}

void cmpKeyWords ()
{
    const char *key_words [COUNT_OF_KEY_WORDS] = {
        "cin", "cout", "auto", "int", "double", "string", "if", "else", "for", "return", "while", "do", "bool", "true", "false"
    };

    const char *built_in_functions [COUNT_OF_BUILT_IN_FUNCTIONS] = {
        "length", "substr", "concat", "find", "sort"
    };

    for (int i = 0; i < COUNT_OF_KEY_WORDS; i++)
    {
        if (strcmp(token.area, key_words[i]) == 0)
        {
            token.type = K_CIN + i;
            return ;
        }
    }


    for (int i = 0; i < COUNT_OF_BUILT_IN_FUNCTIONS; i++)
    {
        if (strcmp(token.area, built_in_functions[i]) == 0)
        {
            token.type = B_LENGTH + i;
            return ;
        }
    }

}

void numberConverter(int base)
{
    token.int_numb = (int) strtol(token.area, NULL, base);
}

void stringEscape (int counter)
{
    if (counter >= 1 && counter <= 255)
        fillToken(counter + '\0');
    else
    {
        fatalError (ERR_StringEscape);
    }
}

void fillToken (char character) {  // naplnit token
    // int extension_tok = 0;

        /// prvni inicializace
    if (token.counter == 0)
    {
        token.area = (char *) customMalloc(3);   /// 2 charactery + \O
        token.sizeof_area = 2;
    }
    else if (token.counter == token.sizeof_area)
    {      /// navyseni kapacity o dvojnasobek
        token.sizeof_area = token.sizeof_area * 2;
        token.area = (char *) customRealloc(token.area, (token.sizeof_area + 1));
    }

    token.area [token.counter] = character;
    token.area [token.counter + 1] = '\0';
    (token.counter)++;

    return ;
}

int scanner () {

    if (ungotToken)             //ungetToken was called
    {
        SWAP(token, oldToken, tToken);
        ungotToken = 0;
        return 0;
    }

    tokenCopy(&oldToken, token);

    initToken();

    int c = 0;
    char array_x [9];
    int counter = 0;
    int base = 0;
    int value = START;
    bool test = true;

    while (test && ( c = fgetc(source)) != EOF)
    {

        switch (value) {
        case START:
                /* Rozpoznani characteru  */
            if (isalpha(c)) {
                value = LETTER;
                 fillToken(c);
                }
            else if (c == '0') {
                value = NUMBER_START_ZERO;
                 fillToken(c);
                break;
                }
            else if (isdigit(c)) {
                value = NUMBER;
                 fillToken(c);
                                }
            else {

        switch(c) {
            case ' ':          // preskocim character mezery
                break;
            case '\t':
                break;                  // preskocim character tabulatoru
            case '\n':
                token.counter_of_lines++;
                break;                  // preskocim character konce radku
            case '/':
                value = DIVIDE;
                break;
            case '\\':
                value = BACKSLASH;
                break;
            case '"':
                value = STRING;
                break;

                         default:  {

                                switch (c)
                                {
                                case '_':
                                    value = UNDERSCORE;
                                    break;
                                case '!':
                                    value = EXCLAMATION_MARK;  //vykřičník
                                    break;
                                case '&':
                                    value = AND;
                                    break;
                                case '|':
                                    value = OR;
                                    break;
                                case '?':
                                    value = QUESTION_MARK;
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
                                        test = false;

                                        switch (c)
                                        {
                                        case ';':
                                            token.type = SEMICOLON;
                                            break;
                                        case ',':
                                            token.type = COMMA;
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
                                                fatalError (ERR_UnknownChar);

                                        }//posleni switch
                                } //předposlední switch
                             fillToken(c);
                            }//default
            }//switch
        }//else


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
/*          else if (isalpha(c) || c == '_')
            {
                fatalError (ERR_LEXICAL);
            }   */
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
            }   /*
            else if (isalpha(c) || c == '_')
            {
                fatalError (ERR_LEXICAL);
            }   */
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
                        fatalError (ERR_NumberShape);
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
            }   /*
            else if (c == '.' || c == '_' || isalpha(c))  /// Lex_an chyba - zadana druha desetinna tecka
            {
                fatalError (ERR_LEXICAL);
            }   */
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
                    fatalError (ERR_NumberShape);
                }

            break;

        case EXP_NUMBER_SIGN:
            if (isdigit(c))
            {
                fillToken(c);
                value = EXP_NUMBER_END;    /// za E jsou cifry
            }
            else
            {
                fatalError (ERR_NumberShape);
            }

            break;

        case EXP_NUMBER_END:
            if (isdigit(c))
                fillToken(c);
/*          else if (isalpha(c) || c == '_' || c == '.')
            {
                fatalError (ERR_LEXICAL);
            }   */
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
                token.type = STRING;    /// ungetc(c, source); - vytvari nekonecny cyklus
                test = false;
            }
            break;

        case STRING_ESCAPE:
            value = STRING;
            counter = 0;

            if (c == 'n')
                fillToken('\n');
            else if (c == 't')
                fillToken('\t');
            else if (c == '\\')
                fillToken('\\');
            else if (c == '"')
                fillToken('\"');
            else if (c == 'x')
                value = ESCAPE_HEXADECIMAL;
            else if (c == '0')
                value = ESCAPE_OCTAL;
            else if (c == 'b')
                value = ESCAPE_BINARY;
            else {
                fatalError (ERR_StringEscape);
            }

            break;

        case ESCAPE_HEXADECIMAL:
            if (counter < 2 && (isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')))
            {
                array_x [counter] = c;
                array_x [counter + 1] = '\0';
                counter++;

                if (counter == 2)
                {
                    counter = (int) strtol(array_x, NULL, 16);
                    stringEscape(counter);
                    value = STRING;
                    counter = 0;
                }
            }
            else
            {
                fatalError (ERR_StringEscape);
            }

            break;

        case ESCAPE_OCTAL:
            if (counter < 3 && (c >= '0' && c <= '7'))
            {
                array_x [counter] = c;
                array_x [counter + 1] = '\0';
                counter++;

                if (counter == 3)
                {
                    counter = (int) strtol(array_x, NULL, 8);
                    stringEscape(counter);
                    value = STRING;
                    counter = 0;
                }
            }
            else
            {
                fatalError (ERR_StringEscape);
            }

            break;

        case ESCAPE_BINARY:
            if (counter < 8 && (c >= '0' && c <= '1'))
            {
                array_x [counter] = c;
                array_x [counter + 1] = '\0';
                counter++;

                if (counter == 8)
                {
                    counter = (int) strtol(array_x, NULL, 2);
                    stringEscape(counter);
                    value = STRING;
                    counter = 0;
                }
            }
            else
            {
                fatalError (ERR_StringEscape);
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
        counter = 0;

        if (c == 'b')
            value = BINARY;
        else if (c == '0')
            value = OCTAL;
        else if (c == 'x')
            value = HEXADECIMAL;
        else {
            fatalError (ERR_NumberShape);
        }

        break;

    case BINARY:
        if (counter == 8 && ((c >= '0' && c <= '1') /*|| isalpha(c)*/ )) {
            fatalError (ERR_NumberEscape);
        }

        if (c >= '0' && c <= '1') {
            fillToken(c);
            counter++;
        }
        else {
            test = false;
            token.type = BINARY;
            base = 2;
            ungetc(c,source);
        }

        break;

    case OCTAL:
        if (counter == 3 && c >= '0' && c <= '7') {
            fatalError (ERR_NumberEscape);
        }

        if (c >= '0' && c <= '7') {
            fillToken(c);
            counter++;
        }
        else {
            test = false;
            token.type = OCTAL;
            base = 8;
            ungetc(c,source);
        }

        break;

    case HEXADECIMAL:
        if (counter == 2 && (isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
            fatalError (ERR_NumberEscape);
        }

        if (isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
            fillToken(c);
            counter++;
        }
        else {
            test = false;
            token.type = HEXADECIMAL;
            base = 16;
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

    case AND:
        if (c == '&') {
            fillToken(c);
            token.type = AND;
        }
        else {
            fatalError (ERR_UnknownChar);
        }

        break;

    case OR:
        if (c == '|') {
            fillToken(c);
            token.type = OR;
        }
        else {
            fatalError (ERR_UnknownChar);
        }

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

        }   // end main switch
    }   // end while





    if (token.area != NULL)
    {
        if (token.type == IDENTIFIER)
            cmpKeyWords();

        else if (token.type == INT_NUMBER)
            numberConverter(10);

        else if (token.type == BINARY || token.type == OCTAL || token.type == HEXADECIMAL)
        {
            numberConverter(base);
            token.type = INT_NUMBER;

            if (token.int_numb <= 0 || token.int_numb > 255)
            {
                fatalError (ERR_LEXICAL);
            }
        }
        else if (token.type == DOUBLE_NUMBER)
        {
            double real_number;
            char *ptr;

            real_number = strtod(token.area, &ptr);
            token.double_numb = real_number;
        }
    }


    if (c == EOF)
    {
        token.type = EOF;

        if (value == COMMENT || value == COMMENT_END)
            fatalError (ERR_Comment);
    }
/* jestli nikdo nepotřebuje token.expression je mo
    if (token.type == INT_NUMBER || token.type== DOUBLE_NUMBER)
        token.expression = IDENTIFIER;
    else
        token.expression = token.type;
*/
    return ERR_None;
}

#ifndef LEX_AN_H
#define LEX_AN_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "error.h"
#include "garbage_collector.h"
#include "ial.h"

#define SWAP(x, y, T) do { T TMP = x; x = y; y = TMP; } while (0)

/** jednotlivé stavy */
enum {
    START,              ///         Určení znaku

    NUMBER,             ///         0..9
    LETTER,             ///         a..z, A..Z
    NUMBER_START_ZERO,
    DEC_NUMBER,         ///         1.234
    DEC_NUMBER_END,     ///         *.123
    EXP_NUMBER,         ///         1e+4 15E8
    EXP_NUMBER_SIGN,    ///         znamenkovy (+/-) 15E+8
    EXP_NUMBER_END,     ///

    SEMICOLON,          ///         ;
    UNDERSCORE,         ///         _   /10

    L_BRACKET,          ///         (
    R_BRACKET,          ///         )
    L_CURLY_BRACKET,    ///         {
    R_CURLY_BRACKET,    ///         }

    PLUS,               ///         +
    MINUS,              ///         -
    INCREMENT,          ///         ++
    DECREMENT,          ///         --
    MULTIPLY,           ///         *
    DIVIDE,             ///         /   /20
    BACKSLASH,          ///

    EXCLAMATION_MARK,   ///         !
    NEGATION,           ///         !=
    AND,                ///         &&
    OR,                 ///         ||
    STRING,             ///         ".."
    STRING_ESCAPE,      ///
    ESCAPE_HEXADECIMAL, ///
    ESCAPE_OCTAL,       ///
    ESCAPE_BINARY,      ///
    COMMA,              ///         ,

    LESS,               ///         <
    GREATER,            ///         >
    C_IN,               ///         >>
    C_OUT,              ///         <<
    ASSIGNMENT,         ///         =
    EQUAL,              ///         ==
    LESS_EQUAL,         ///         <=
    GREATER_EQUAL,      ///         >=

    LINE_COMMENT,       ///         // ...
    COMMENT,            ///         /* ...
    COMMENT_END,        ///         ... */

    TOK_NULL,           ///         prazdny/neinicializovany token  */

    BINARY,             ///         dvojkove cislo
    OCTAL,              ///         osmickove cislo
    HEXADECIMAL,        ///         hexadecimalni cislo

        /// typy tokenu
    IDENTIFIER = 60,    ///
    KEY_WORD,           ///
    BUILT_IN_FUNCTION,  ///
    INT_NUMBER,         ///
    DOUBLE_NUMBER,      ///

        /// klicova slova
    K_CIN = 70,         ///
    K_COUT,             ///
    K_AUTO,             ///
    K_INT,              ///
    K_DOUBLE,           ///
    K_STRING,           ///
    K_IF,               ///
    K_ELSE,             ///
    K_FOR,              ///
    K_RETURN,           ///
    K_WHILE,            ///
    K_DO,               ///
    K_BOOL,             ///
    K_TRUE,             ///
    K_FALSE,            ///

        /// vestavene funkce
    B_LENGTH = 85,      ///
    B_SUBSTR,           ///
    B_CONCAT,           ///
    B_FIND,             ///
    B_SORT,             ///

    MAX_OF_ENUM         ///

};

/*
union unie {
    int int_number;
    double double_number;
    char *area;
} unie;
*/


typedef struct {
    char *area;
    int int_numb;
    double double_numb;

    int type;
    int counter_of_lines;

    int counter;
    int sizeof_area;
} tToken;


#define COUNT_OF_KEY_WORDS 15
#define COUNT_OF_BUILT_IN_FUNCTIONS 5

    /// globalni promenne
tToken token, oldToken;
FILE *source;

    /// prototypy funkci

void ungetToken();               //ungets the latest token, can be used only once in a row
//void tokenSwap();
int tokenCopy (tToken *dst, tToken src);   //"copies" the contents of src token to the dst token
void initToken (); // function for inicialization Token .. it's called when you need delete old variables
void fillToken (char character); // comes some character and function saves it to variable
int scanner ();  // its scanner, heart of lex_an ... it's automata



int ungotToken;                 //TODO: inicializovať na 0 >_<

#endif

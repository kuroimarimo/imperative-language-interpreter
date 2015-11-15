#ifndef LEX_AN_H
#define LEX_AN_H

/** jednotlivé stavy */
enum {
    START,              /// 0       Určení znaku

    NUMBER,             /// 1       0..9
    LETTER,             /// 2       a..z, A..Z
    NUMBER_START_ZERO,
    DEC_NUMBER,         /// 4       1.234
    DEC_NUMBER_END,     /// 5       *.123
    EXP_NUMBER,         /// 6       1e+4 15E8
    EXP_NUMBER_SING,    /// 7       znamenkovy (+/-) 15E+8
    EXP_NUMBER_END,     /// 8

    SEMICOLON,          /// 9       ;
    UNDERSCORE,         /// 10      _

    L_BRACKET,          /// 11      (
    R_BRACKET,          /// 12      )
    L_SQUARE_BRACKET,   /// 13      [
    R_SQUARE_BRACKET,   /// 14      ]
    L_CURLY_BRACKET,    /// 15      {
    R_CURLY_BRACKET,    /// 16      }

    PLUS,               /// 17      +
    MINUS,              /// 18      -
    MULTIPLY,           /// 19      *
    DIVIDE,             /// 20      /
    MODULO,             /// 21      %

    EXCLAMATION_MARK,   /// 22      !
    NEGACE,             /// 23      !=
    QUESTION_MARK,      /// 24      ?
    STRING,             /// 25      ".."
    STRING_ESCAPE,      /// 26
    STRING_ESCAPE2,     /// 27
    COMMA,              /// 28      ,

    LESS,               /// 29      <
    GREATER,            /// 30      >
    C_IN,               /// 31      <<
    C_OUT,              /// 32      >>
    ASSIGNMENT,         /// 33      =
    EQUAL,              /// 34      ==
    LESS_ROVNO,         /// 35      <=
    GREATER_ROVNO,      /// 36      >=

    LINE_COMMENT,       /// 37      // ...
    COMMENT,            /// 38      /* ...
    COMMENT_KON,        /// 39      ... */

    ANOTHER_CHAR,       /// 40      ostatni znaky


    TOK_NULL,           /// 41      prazdny/neinicializovany token  */

        /// typy tokenu
    IDENTIFIER,         /// 42
    KEY_WORD,           /// 43
    BUILT_IN_FUNCTION,  /// 44
    INT,                /// 45
    DOUBLE,             /// 46
    AUTO                /// 47

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
    int counter;
    int sizeof_area;
    int type;
    int counter_of_lines;

    union {
        int int_number;
        double double_number;
        char *area;
        } unie;
} tToken;



#define COUNT_OF_KEY_WORDS 10
#define COUNT_OF_BUILT_IN_FUNCTIONS 5

    /// globalni promenne
tToken token;

    /// prototypy funkci
int scanner (FILE *source);

#endif

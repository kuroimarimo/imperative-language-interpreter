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
    STRING,             /// 25      "
    COMMA,              /// 26      ,

    LESS,               /// 27      <
    GREATER,            /// 28      >
    C_IN,               /// 29      <<
    C_OUT,              /// 30      >>
    ASSIGNMENT,         /// 31      =
    EQUAL,              /// 32      ==
    LESS_ROVNO,         /// 33      <=
    GREATER_ROVNO,      /// 34      >=

    LINE_COMMENT,       /// 35      // ...
    COMMENT,            /// 36      /* ...
    COMMENT_KON,        /// 37      ... */

    ANOTHER_CHAR,       /// 38      ostatni znaky


    TOK_NULL,           /// 39      prazdny/neinicializovany token  */

        /// typy tokenu
    IDENTIFIER,         /// 40
    KEY_WORD,           /// 41
    BUILT_IN_FUNCTION,  /// 42
    INT,                /// 43
    DOUBLE,             /// 44
    AUTO                /// 45

};

typedef struct {
    char *area;
    int counter;
    int sizeof_area;
    int type;
    int counter_of_lines;
} tToken;



#define COUNT_OF_KEY_WORDS 10
#define COUNT_OF_BUILT_IN_FUNCTIONS 5

    /// globalni promenne
tToken token;

    /// prototypy funkci
int scanner (FILE *source);

#endif

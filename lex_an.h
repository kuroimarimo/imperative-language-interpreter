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
    EXP_NUMBER_SIGN,    /// 7       znamenkovy (+/-) 15E+8
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
    INCREMENT,          /// 19      ++
    DECREMENT,          /// 20      --
    MULTIPLY,           /// 21      *
    DIVIDE,             /// 22      /
    MODULO,             /// 23      %

    EXCLAMATION_MARK,   /// 24      !
    NEGATION,           /// 25      !=
    QUESTION_MARK,      /// 26      ?
    STRING,             /// 27      ".."
    STRING_ESCAPE,      /// 28
    STRING_ESCAPE_x1,   /// 29
    STRING_ESCAPE_x2,   /// 30
    COMMA,              /// 31      ,

    LESS,               /// 32      <
    GREATER,            /// 33      >
    C_IN,               /// 34      >>
    C_OUT,              /// 35      <<
    ASSIGNMENT,         /// 36      =
    EQUAL,              /// 37      ==
    LESS_EQUAL,         /// 38      <=
    GREATER_EQUAL,      /// 39      >=

    LINE_COMMENT,       /// 40      // ...
    COMMENT,            /// 41      /* ...
    COMMENT_END,        /// 42      ... */

    ANOTHER_CHAR,       /// 43      ostatni znaky


    TOK_NULL,           /// 44      prazdny/neinicializovany token  */

        /// typy tokenu
    IDENTIFIER,         /// 45
    KEY_WORD,           /// 46
    BUILT_IN_FUNCTION,  /// 47
    INT_NUMBER,         /// 48
    DOUBLE_NUMBER,      /// 49

        /// klicova slova
    K_AUTO,             /// 50
    K_CIN,              ///
    K_COUT,             ///
    K_DOUBLE,           ///
    K_ELSE,             ///
    K_FOR,              ///
    K_IF,               ///
    K_INT,              ///
    K_RETURN,           ///
    K_STRING,           ///
    K_WHILE,            ///
    K_DO,               /// 61

        /// vestavene funkce
    B_LENGTH,           /// 62
    B_SUBSTR,           ///
    B_CONCAT,           ///
    B_FIND,             ///
    B_SORT,             /// 66

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


#define COUNT_OF_KEY_WORDS 12
#define COUNT_OF_BUILT_IN_FUNCTIONS 5

    /// globalni promenne
tToken token, oldToken;

    /// prototypy funkci
int scanner (FILE *source);
void ungetToken();               //ungets the latest token, can be used only once in a row
void tokenSwap();
int tokenCopy (tToken *dst, tToken src);   //"copies" the contents of src token to the dst token
void initToken (); // function for inicialization Token .. it's called when you need delete old variables
void fillToken (char character); // comes some character and function saves it to variable 
int scanner (FILE *source);  // its scanner, heart of lex_an ... it's automata 


int ungotToken;                 //TODO: inicializovať na 0 >_<

#endif

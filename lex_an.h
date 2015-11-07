
/** jednotlivé stavy */
enum {
    START,        /// Určení znaku

    NUMBER,          /// 0..9
    LETTER,        /// a..z, A..Z
    NUMBER_START_ZERO, 
    DEC_NUMBER,      /// 1.234
    DEC_NUMBER_END,    /// *.123
    EXP_NUMBER,      /// 1e+4 15E8
    EXP_NUMBER_SING,   /// znamenkovy (+/-) 15E+8
    EXP_NUMBER_END,  ///

    SEMICOLON,       /// ;
    UNDERSCORE,     /// _
    SPACE,         /// ' '
    TABULATOR,      /// '\t'
    END_OF_LINE,    /// '\n'

    L_BRACKET,      /// (
    R_BRACKET,      /// )
    L_SQUARE_BRACKET,     /// [
    R_SQUARE_BRACKET,     /// ]
    L_CURLY_BRACKET,     /// {
    R_CURLY_BRACKET,     /// }

    PLUS,           /// +
    MINUS,          /// -
    MULTIPLY,           /// *
    DIVIDE,         /// /
    MODULO,         /// %

    EXCLAMATION_MARK,      /// !
    NEGACE,         /// !=
    QUESTION_MARK,        /// ?
    STRING,        /// "
    COMMA,          /// ,
/*    TECKA,        /// .   */
/*  DVOJTECKA       /// :   */

    LESS,          /// <
    GREATER,          /// >
    C_IN,           /// <<
    C_OUT,          /// >>
    EQUAL,       /// =
    LESS_ROVNO,    /// <=
    GREATER_ROVNO,    /// >=

    LINE_COMMENT,   /// // ...
    COMMENT,       /// /* ...
    COMMENT_KON,   /// ... */

    ANOTHER_CHAR,       /// ostatni znaky


    TOK_NULL,       /// prazdny/neinicializovany token  */

        /// typy tokenu
    IDENTIFIER,
    KEY_WORD,
    BUILT_IN_FUNCTION,
    INT,
    DOUBLE,
    AUTO

};

typedef struct {
    char *area;
    int counter;
    int type;
    int counter_of_lines;
} tToken;



#define COUNT_OF_KEY_WORDS 10
#define COUNT_OF_BUILT_IN_FUNCTIONS 5

    /// globalni promenne
tToken token;

    /// prototypy funkci
int scanner (FILE *source);


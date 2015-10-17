
/** jednotlivé stavy */
enum {
    POCATEK,        /// Určení znaku

    CISLO,          /// 0..9
    PISMENO,      /// a..z, A..Z

    STREDNIK,       /// ;
    PODTRZITKO,     /// _
    MEZERA,         /// ' '
    TABULATOR,      /// '\t'
    KONEC_RADKU,    /// '\n'

    L_ZAVORKA,      /// (
    P_ZAVORKA,      /// )
    L_HRAN_ZAV,     /// [
    P_HRAN_ZAV,     /// ]
    L_SLOZ_ZAV,     /// {
    P_SLOZ_ZAV,     /// }

    PLUS,           /// +
    MINUS,          /// -
    KRAT,           /// *
    DELENO,         /// /
    MODULO,         /// %

    VYKRICNIK,      /// !
    OTAZNIK,        /// ?
    RETEZEC,        /// "
    CARKA,          /// ,
/*    TECKA,        /// .   */
/*  DVOJTECKA       /// :   */

    ROZ_MENSI,      /// < nebo <<
    ROZ_VETSI,      /// > nebo >>
    MENSI,          /// <
    VETSI,          /// >
    ROVNITKO,       /// =

    JINY_ZNAK,       /// ostatni znaky

    TOK_NULL,       /// neinicializovany token  */
    TOK_PRAZDNY,    /// prazdny token
    TOK_HOTOVY,      /// naplneny token

    IDENTIFIKATOR,  /// typ tokenu
    INT,
    DOUBLE,
    AUTO

};

typedef struct {
    char *obsah;
    int pocitadlo;
    int stav;
    int typ;
} tToken;



/**
auto
int
double
cin
cout
if
else
for
string
return
*/

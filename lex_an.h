
/** jednotlivé stavy */
enum {
    POCATEK,        /// Určení znaku

    CISLO,          /// 0..9
    PISMENO,        /// a..z, A..Z
    DES_CISLO,      /// 1.234
    EXP_CISLO,      /// 1e+4 15E8
    EXP_CISLO_ZN,   /// znamenkovy (+/-) 15E+8
    EXP_CIS_KONEC,  ///

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
    NEGACE,         /// !=
    OTAZNIK,        /// ?
    RETEZEC,        /// "
    CARKA,          /// ,
/*    TECKA,        /// .   */
/*  DVOJTECKA       /// :   */

    MENSI,          /// <
    VETSI,          /// >
    C_IN,           /// <<
    C_OUT,          /// >>
    ROVNITKO,       /// =
    MENSI_ROVNO,    /// <=
    VETSI_ROVNO,    /// >=

    RAD_KOMENTAR,   /// // ...
    KOMENTAR,       /// /* ...
    KOMENTAR_KON,   /// ... */

    JINY_ZNAK,       /// ostatni znaky


    TOK_NULL,       /// prazdny/neinicializovany token  */

        /// typy tokenu
    IDENTIFIKATOR,
    KLICOVE_SLOVO,
    VEST_FUNKCE,
    INT,
    DOUBLE,
    AUTO

};

typedef struct {
    char *obsah;
    int pocitadlo;
    int typ;
} tToken;



#define POCET_KLIC_SLOV 10
#define POC_VEST_FUNKCI 5

    /// globalni promenne
tToken token;

    /// prototypy funkci
int scanner (FILE *soubor);


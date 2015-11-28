/*
 V: Opravila som, co sa dalo (cez 50 errorov), stale to hadze warningy a errory. Pre celkove opravenie by to vyzadovalo, aby som to takmer cele prepisala...
 Mas do konca vikendu na to, aby si to dokoncil. Slovom "dokoncil" myslim, aby to bolo prelozitelne na eve bez akychkolvek warningov, semanticky spravne, prehladne napisane a *VHODNE* okomentovane. 
 Ak to nezvladnes, tak to urobim ja a budu ti strhnute body. 
 
 Pridane komentare 'ERROR' oznacuju chybovy stav, ktory treba osetrit cez globalnu premennu a uz existujuce funkcie. 
 Ostatne naznacuju niektore z kritickych miest.
 Prosim neries elegantne dealokovavanie pamati.
 
 K hlbsiemu kontrolovaniu logickej spravnosti som sa nedostala.
 */

/*
-----------------
error propojka

return prec.SA ukazatel na výsledek; return SA;
*/
#include "prsa.h"

//--------------------------//
//          ERROR           //
//--------------------------//
/*void Error(int errcode) {
    // uvolneni znameho
    // v budoucnu optimalne pomoci globalniho seznamu
    Dispose(Z);
    DisposeList(L);
    free(suc1);
    free(suc2);
    free(suc3);

    // volani ukonceni
    errorState.state = errcode;
    errorState.line = token.counter_of_lines;
    fatalError(errorState);
}*/

char OperatorToIndex (tExpr* op) {
    switch (op->type) {
        case MULTIPLY:      return 0;   // *    = 0
        case DIVIDE:        return 1;   // /    = 1
        case PLUS:          return 2;   // +    = 2
        case MINUS:         return 3;   // -    = 3
        case GREATER:       return 4;   // >    = 4
        case GREATER_EQUAL: return 5;   // >=   = 5
        case LESS:          return 6;   // <    = 6
        case LESS_EQUAL:    return 7;   // <=   = 7
        case EQUAL:         return 8;   // ==   = 8
        case NEGATION:      return 9;   // !=   = 9
        case L_BRACKET:     return 10;  // (    = 10
        case R_BRACKET:     return 11;  // )    = 11
        case DOUBLE_NUMBER:
        case INT_NUMBER:
        case STRING:
        case IDENTIFIER:    return 12;  // i    = 12 <----- dodelat
        case SEMICOLON:     return 13;  // ;    = 13
        default:
            return -1;                                                              //V: ERROR
    }
}

//--------------------------//
//        NEXT TOKEN        //
//--------------------------//
tExpr* NextToken() {
    FILE * srcFile;
    scanner(srcFile);
    char * str;
    tExpr * expression;
    static int cond_counter;
    if ((expression = malloc(sizeof(tExpr))) == NULL)
        return NULL; // chyba
    expression->terminal = TERMINAL;
    expression->type = token.type;
    switch (token.type) {
        case L_BRACKET: 
            cond_counter++; 
        break;
        case R_BRACKET: 
            cond_counter--; 
            if (cond_counter > 0) {
                cond_counter = 0;
                return NULL;          //CALL FILIP; // <<<<<<<<<<<<<<<<<<< ------- AARGH
                                                                                    //V: TODO: Zavolat Filipa
            }
        break;
        case INT_NUMBER: 
            if ((expression->data = malloc(sizeof(int))) == NULL)
                return NULL;                                                        //V: ERROR
            
            expression->data->int_value = token.int_numb;
        break;
        case DOUBLE_NUMBER:
            if ((expression->data = malloc(sizeof(double))) == NULL)
                return NULL;                                                        //V: ERROR
            expression->data->double_value = token.double_numb;
        break;
        case STRING:
        case IDENTIFIER:
            if ((str = malloc(strlen(token.area)+1)) == NULL)
                return NULL;                                                        //V: ERROR
            expression->data->string_value = str;
            strcpy(expression->data->string_value, token.area);
        break;
    }
    return expression;
}
//--------------------------//
//      ZASOBNIK            //
//--------------------------//
void Init (tList *Z) {                                          // inicializace
    Z->First = NULL;
}
void Dispose (tList *Z) {
    tElemPtr ukazatel = NULL;                                   // deklarace pro predani ukazatele  
    while(Z->First != NULL) {                                   // cyklus pro free() zasobniku
        ukazatel = Z->First->ptr;
        free(Z->First);
        Z->First = ukazatel;
    }
    Init(Z);                                                    // reinicializace zasobniku
}
tExpr* FirstTerminal (tList *Z) {
    tExpr * val = NULL;
    if (Z->First != NULL) {
        val = Z->First->data;
        while (val->terminal == NETERMINAL)                      // hledam terminal
            if (Z->First->ptr != NULL) 
                val = Z->First->data;                           // aktualni terminal
            else NULL;                                                                  //V: ERROR
    }
    return val;
}
void Push (tList *Z, tExpr *val) {
    tElemPtr ukazatel = NULL;
    if(Z->First != NULL) ukazatel = Z->First;
    tElemPtr struktura = malloc(sizeof(struct tElem));          // alokace struktury
    if(struktura != NULL) {                                     // kontrola alokace
        Z->First = struktura;                                   // prirazeni a naplneni
        struktura->data = val;
        struktura->ptr = ukazatel;
    }
    else  return;                                                                       //V: ERROR
}
tExpr* Pop (tList *Z) {
    tExpr* val = NULL;
    if (Z->First != NULL) {
        val = Z->First->data;                                   // pop hodnoty
        tElemPtr ukazatel = NULL;                               // deklarace pro presun ukazatelu
        if (Z->First->ptr != NULL) ukazatel = Z->First->ptr;    // uchovani ukazatele
        free(Z->First);
        Z->First = ukazatel;                                    // posunuti fronty
    }
    return val;
}
//--------------------------//
//     INSTRUCTION ARRAY    //
//--------------------------//

void expandInstrList(tInstruction * array, int size)
{
    instructionList = realloc(instructionList, size * 2 * sizeof(tInstruction));
    if (!instructionList)
    {
                                                                                        //V: ERROR malloc failed
        return;
    }
}

void addInstruction(tInstruction * instr)
{
    if (instructionList.lenght < (instructionList.occupied + 1))
        instructionList.array = expandInstrList(instructionList.array, instructionList.lenght);
    instructionList.array[instructionList.occupied] = * instr;
    ++instructionList.occupied;
}

//--------------------------//
//      GRAMATIKA           //
//--------------------------//
int PrecG(tExpr *suc1, tExpr *suc2, tExpr *suc3) {
    switch (suc1->terminal) {
        case NETERMINAL:
            if (suc3 == NULL)
                return -1;                                                              //V: ERROR
            if (suc3->type == NETERMINAL)
            {
                switch (suc2->type) {
                    case PLUS:          return 1;
                    case MINUS:         return 2;
                    case MULTIPLY:      return 3;
                    case DIVIDE:        return 4;
                    case GREATER:       return 5;
                    case GREATER_EQUAL: return 6;
                    case LESS:          return 7;
                    case LESS_EQUAL:    return 8;
                    case EQUAL:         return 9;
                    case NEGATION:        return 10;
                }
            }
        break;
        case TERMINAL:
            switch (suc1->type) {
                case STRING:
                case IDENTIFIER:
                case DOUBLE_NUMBER:
                case INT_NUMBER:
                    return 11;
                case L_BRACKET:
                    if (suc2 == NULL || suc3 == NULL)
                        return -1;                                                       //V: ERROR
                    if (suc2->terminal == NETERMINAL)
                        if (suc3->type == R_BRACKET)
                            return 12;
                break;
            }
    }
    return 0;
}

//------------------------------//
//        GENEROVANI 3AK        //
//------------------------------//
void TrojAdres(int gramatika, tExpr* input1, tExpr* input2, tExpr* output)
{
    int operator, type;
    switch (gramatika)
    {
        case 1: operator = OP_SUM; break;
        case 2: operator = OP_MINUS; break;
        case 3: operator = OP_MUL; break;
        case 4: operator = OP_DIV; break;
        case 5: operator = OP_GT; break;
        case 6: operator = OP_GE; break;
        case 7: operator = OP_LT; break;
        case 8: operator = OP_LE; break;
        case 9: operator = OP_EQUALS; break;
        case 10: operator = OP_DIFF; break;
                                                                                        //V: Doplnit default.
    }
    switch (input1->type)
    {
        case INT_NUMBER: type = INT_NUMBER; break;
        case DOUBLE_NUMBER: type = DOUBLE_NUMBER; break;
        case STRING: type = STRING; break;                                              //V: Doplnit default.
    }
    tInstruction * instrBeingAdded;
    if ((instrBeingAdded = malloc(sizeof(tInstruction))) != NULL) //if FALSE -> error
    {
        instrBeingAdded->operator = operator;                                           //V: Warning, pretoze 'operator' nemusi byt inicializovany kvoli chubajucemu defaultu vo switchi.
        instrBeingAdded->type = type;                                                   //V: To iste.
        instrBeingAdded->input1 = input1->data;
        instrBeingAdded->input2 = input2->data;
        instrBeingAdded->output = output->data;
        instructionList[instructionList.occupied] = addInstruction(instrBeingAdded);
    }
}
//------------------------------//
//   PREC. SEMANTICKA ANALYZA   //
//------------------------------//
tExpr* SemId(tExpr* identifier) {                                   //V: Cele zle. Preco je vyhladavane v hash. tab podla DAT?
    hashElem* element;                                              //V: Prepisat na pracu s polom.
    element = findElem(tabulka, identifier->data);                  //V: K comu je vlastne dobry typ tExpr, ked mame hashElem.
    free(identifier->data);
    if (element == NULL)
        return NULL;         // Error(ERR_UndefinedVariable);
    identifier->data = &element->data.value;                       //V: Zjednotit tSymbolValue a tUnionData?
    identifier->type = element->data.type;
    return identifier;
}

tExpr* SemA(tExpr *expr1, tExpr *expr2, tExpr *expr3, int gramatika) {

    if (expr1->type == IDENTIFIER)
        return SemId(expr1);
if (gramatika != 11)
{                                              // !!!!!!!!!!!!!!!! ACHTUNG
    if (expr2->type == IDENTIFIER)
        return SemId(expr2);
    if (expr3->type == IDENTIFIER)
        return SemId(expr3);
    if ((expr1->type == STRING && (expr3->type == DOUBLE_NUMBER ||
        expr3->type == INT_NUMBER)) || ((expr1->type == DOUBLE_NUMBER ||
        expr1->type == INT_NUMBER) && expr3->type == STRING))
        return NULL;                                                                        //Error(ERR_IncompatibleExpr);
}
    tExpr *struktura;                                                                       //V: pomenovanie premenne -> informacna hodnota = 0
    if ((struktura = malloc(sizeof(tExpr))) == NULL)
        return NULL;                                                                        //Error(ERR_AllocFailed);
    struktura->terminal = NETERMINAL;
    switch (gramatika) {
        case 1:
        case 2:
        case 3:
        case 4:
            if (expr1->type == DOUBLE_NUMBER || expr3->type == DOUBLE_NUMBER) {
                struktura->type = DOUBLE_NUMBER;
                if ((struktura->data = malloc(sizeof(double))) == NULL)
                    return NULL;                                                            //Error(ERR_AllocFailed);
            }
            else if (expr1->type == STRING || expr3->type == STRING)
                Error(ERR_IncompatibleExpr);
            else {
                struktura->type = INT_NUMBER;
                if ((struktura->data = malloc(sizeof(int))) == NULL)
                    Error(ERR_AllocFailed);
            }
            TrojAdres(gramatika, expr1->data, expr3->data, struktura->data);                //V: TODO: predavanie struktura->data
            break;
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
            if (expr1->type == INT_NUMBER && expr3->type == DOUBLE_NUMBER)
            {
                int docasna_prom = *((int *)expr1->data);                                   //V: preco nedeklarovat docasnu premennu hore?
                if ((expr1->data = realloc(expr1->data, sizeof(double))) == NULL)
                    return NULL;                                                            //Error(ERR_AllocFailed);
                expr1->data->int_value = docasna_prom;
            }
            if (expr1->type == DOUBLE_NUMBER && expr3->type == INT_NUMBER)
            {
                int docasna_prom = *((int *)expr3->data);
                if ((expr3->data = realloc(expr3->data, sizeof(double))) == NULL)
                    Error(ERR_AllocFailed);
                expr3->data->double_value = docasna_prom;
            }
            if ((expr1->type == INT_NUMBER && expr3->type == INT_NUMBER) ||
            (expr1->type == DOUBLE_NUMBER && expr3->type == DOUBLE_NUMBER) ||
            (expr1->type == STRING && expr3->type == STRING))
            {
                struktura->type = INT_NUMBER;
                if ((struktura->data = malloc(sizeof(int))) == NULL)
                    Error(ERR_AllocFailed);
                TrojAdres(gramatika, expr1->data, expr3->data, struktura->data);
            }
            else return 4;                                                                  //V: funkcia ma vracat typ tExpr *...
        break;
        case 11:
            switch (expr1->type) {
                case INT_NUMBER: struktura->type = INT_NUMBER; break;
                case DOUBLE_NUMBER: struktura->type = DOUBLE_NUMBER; break;
                case STRING: struktura->type = STRING; break;
                default: Error(ERR_IncompatibleExpr);
            }
            TrojAdres(gramatika, expr1->data, NULL, struktura->data);
        break;
        case 12:
            switch (expr2->type) {
                case INT_NUMBER: struktura->type = INT_NUMBER; break;
                case DOUBLE_NUMBER: struktura->type = DOUBLE_NUMBER; break;
                case STRING: struktura->type = STRING; break;
                default: Error(ERR_IncompatibleExpr);
            }
            TrojAdres(gramatika, expr2, NULL, struktura);
        break;
    }
    return struktura;
}
//------------------------------//
//  PRECEDENCNI SYNTAX ANALYZA  //
//------------------------------//
int PrecedencniSA ()
{
    int val, gramatika;             // promenne
    tExpr * input, * suc1, * suc2, * suc3, * top, * start;
    tList * Z;                                           // zasobnik
    Init(Z);
    if ((start = malloc(sizeof(tExpr)))== NULL)
        Error(ERR_AllocFailed);
    start->type = SEMICOLON;
    start->terminal = TERMINAL;
    Push(Z, start);
    input = NextToken();                                    // input
    top = FirstTerminal(Z);                             // nacteni pocatku zasobniku
    while (top->type != SEMICOLON && input->type != SEMICOLON)      // dokud není vstup i zasobnik prazdny
    {
        top = FirstTerminal(Z);                         // nacti dalsi terminal
        switch (precedencni_tabulka[OperatorToIndex(top)][OperatorToIndex(input)])  // najdi pravidlo v prec. tabulce
        {
            case '=':                                   // pokud se rovnaji
                Push(Z, input);                         // zapis do zasobniku
                input = NextToken();                    // nacti dalsi znak na vstupu
                break;
            case '<':             // pokud ma zasobnik mensi prioritu nez vstup (dle tabulky)
                tExpr * priority = malloc(sizeof(tExpr));
                                            //if malloc failed ERROR
                priority->type = 5948;
                Push(Z, priority);                      // zapis do zasobniku znak <
                Push(Z, input);                         // zapis vstup
                input = NextToken();                    // nacti dalsi vstup
                break;
            case '>':                                   // pokud ma zasobnik vetsi prioritu nez vstup (dle tabulky)
                do {
                    suc3 = suc2;                        // zapis 3. predchudce
                    suc2 = suc1;                        // zapis 2. predchudce
                    suc1 = top;                         // zapis prvniho predchudce
                    top = Pop(Z);                       // vytahni prvek na top
                } while (top->type != 5948);             // dokud v promennych nemam cely retezec, pro ktery hledam pravidlo
                gramatika = PrecG(suc1, suc2, suc3);
                if (top->type == 5948 && gramatika != 0) { 
                    free(top);
                    top = SemA(suc1, suc2, suc3, gramatika);
                    Push(Z, top);   // kontrola na nalezene pravidlo, pokud existuje, prepise na E -> neterminal
                }
                else Error(ERR_SYNTAX);
                free(suc1);
                free(suc2);
                free(suc3);
                suc3 = suc2 = suc1 = NULL;
                break;
            case ' ':                                   // chyba ve vyrazu
                return -1;                                                                  //Error(ERR_SYNTAX);
                break;
        }
    }
    Dispose(Z);                                         // uvolneni zasobniku
}

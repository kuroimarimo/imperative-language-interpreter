/*
-----------------
error propojka

return prec.SA ukazatel na výsledek; return SA;
*/
#include "prsa.h"
//--------------------------//
//          ERROR           //
//--------------------------//
void Error(int errcode) {
    // uvolneni znameho
    // v budoucnu optimalne pomoci globalniho seznamu
    Dispoze(Z);
    DisposeList(L);
    free(suc1);
    free(suc2);
    free(suc3);

    // volani ukonceni
    errorState.state = errcode;
    errorState.line = token.counter_of_lines;
    fatalError(errorState);
}
//--------------------------//
//        NEXT TOKEN        //
//--------------------------//
tExpr* NextToken() {
    FILE* srcFile;
    scanner(srcFile);
    char* str;
    tExpr* struktura;
    static int cond_counter;
    if ((struktura = malloc(sizeof(tExpr))) == NULL)
        Error(ERR_AllocFailed); // chyba
    struktura->terminal = TERMINAL;
    struktura->type = token.type;
    switch (token.type) {
        case L_BRACKET: 
            cond_counter++; 
        break;
        case R_BRACKET: 
            cond_counter--; 
            if (cond_counter > 0) {
                cond_counter = 0;
                CALL FILIP; // <<<<<<<<<<<<<<<<<<< ------- AARGH
            }
        break;
        case INT_NUMBER: 
            if ((struktura->data = malloc(sizeof(int))) == NULL)
                Error(ERR_AllocFailed);
            *struktura->data = token.int_numb;
        break;
        case DOUBLE_NUMBER:
            if ((struktura->data = malloc(sizeof(double))) == NULL)
                Error(ERR_AllocFailed);
            *struktura->data = token.double_numb;
        break;
        case STRING:
        case IDENTIFIER:
            if ((str = malloc(strlen(token.area)+1)) == NULL)
                Error(ERR_AllocFailed); // chyba
            struktura->data = str;
            strcpy(struktura->data, token.area);
        break;
    }
    return struktura;
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
    tExpr* val = NULL;
    if (Z->First != NULL) {
        val = Z->First->data;
        while (val.terminal == NETERMINAL)                                      // hledam terminal
            if (Z->First->ptr != NULL) 
                val = Z->First->data;                           // aktualni terminal
            else Error(ERR_SYNTAX);
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
    else Error(ERR_AllocFailed);
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
//      LINEARNI SEZNAM     //
//--------------------------//
void InitList (tInstrList *L) {
    L->last = NULL;     
    L->first = NULL;
}
void DisposeList (tInstrList *L) {
    tInstrListElem *ukazatel = NULL;    
    while(L->first != NULL) {
        ukazatel = L->first->next;
        free(L->first);
        L->first = ukazatel;
    }
    InitList(L);
}
void InsertLast (tInstrList *L, tInstruction *data) {
    tInstrListElem* ukazatel = NULL;
    if(L->last != NULL) ukazatel = L->last;
    tInstrListElem* struktura = malloc(sizeof(tInstrListElem));
    if(struktura != NULL) {
        L->last = struktura;
        L->last->data = data;
        L->last->next = NULL;
        ukazatel->next = L->last;
    }
    else Error(ERR_AllocFailed);
}
tInstruction* GetFirst (tInstrList *L) {
    tInstrListElem* ukazatel = NULL;
    if (L->first == NULL) return NULL;
    if (L->first->next != NULL) ukazatel = L->first->next;
    if (L->first == L->last) L->last = NULL;
    tInstruction* data = NULL;
    data = L->fist->data;
    free(L->first);
    L->first = ukazatel;
    return data;
}   
//--------------------------//
//      GRAMATIKA           //
//--------------------------//
int PrecG(tExpr *suc1, tExpr *suc2, tExpr *suc3) {
    switch (suc1->terminal) {
        case NETERMINAL:
            if (suc3 == NULL)
                Error(ERR_SYNTAX);
            if (suc3->type == NETERMINAL) {
                switch (suc2->type) {
                    case PLUS:          return 1;
                    case MINUS:         return 2;
                    case MULTIPLY:      return 3;
                    case DIVIDE:        return 4;
                    case GREATER:       return 5;
                    case GREATER_ROVNO: return 6;
                    case LESS:          return 7;
                    case LESS_ROVNO:    return 8;
                    case EQUAL:         return 9;
                    case NEGACE:        return 10;
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
                        Error(ERR_SYNTAX);
                    if (suc2->terminal == NETERMINAL)
                        if (suc3->type == R_BRACKET)
                            return 12
                break;
            }
    }
    return 0;
}
//------------------------------//
//        GENEROVANI 3AK        //
//------------------------------//
void TrojAdres(int gramatika, tExpr* input1, tExpr* input2, tExpr* output) {
    int operator, type;
    switch (gramatika) {
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
    }
    switch (input1.type) {
        case INT_NUMBER: type = INT_NUMBER; break;
        case DOUBLE_NUMBER: type = DOUBLE_NUMBER; break;
        case STRING: type = STRING; break;
    }
    tInstruction* predavani;
    if ((predavani = malloc(sizeof(tInstruction))) == NULL)
        Error(ERR_AllocFailed);
    predavani->operator = operator;
    predavani->type = type;
    predavani->input1 = input1->data;
    predavani->input2 = input2->data;
    predavani->output = output->data;
    InsertLast(L, predavani);
}
//------------------------------//
//   PREC. SEMANTICKA ANALYZA   //
//------------------------------//
tExpr* SemId(tExpr* identifier) {
    hashElem* element;
    element = findElem(tabulka, identifier.data);
    free(identifier.data)
    if (element == NULL)
        Error(ERR_UndefinedVariable); // nedefinovana prom
    identifier.data = &element->data.value;
    identifier.type = element->data.type;
    return identifier;
}

tExpr* SemA(tExpr *expr1, tExpr *expr2, tExpr *expr3, int gramatika) {

    if (expr1.type == IDENTIFIER)
        SemId(expr1);
if (gramatika != 11) { // !!!!!!!!!!!!!!!! ACHTUNG
    if (expr2.type == IDENTIFIER)
        SemId(expr2);
    if (expr3.type == IDENTIFIER)
        SemId(expr3);
    if ((expr1.type == STRING && (expr3.type == DOUBLE_NUMBER ||
        expr3.type == INT_NUMBER)) || ((expr1.type == DOUBLE_NUMBER ||
        expr1.type == INT_NUMBER) && expr3.type == STRING))
        Error(ERR_IncompatibleExpr);
}
    tExpr *struktura;
    if ((struktura = malloc(sizeof(tExpr))) == NULL)
        Error(ERR_AllocFailed);
    struktura->terminal = NETERMINAL;
    switch gramatika {
        case 1:
        case 2:
        case 3:
        case 4:
            if (expr1.type == DOUBLE_NUMBER || expr3.type == DOUBLE_NUMBER) {
                struktura->type = DOUBLE_NUMBER;
                if ((struktura->data = malloc(sizeof(double))) == NULL)
                    Error(ERR_AllocFailed);
            }
            else if (expr1.type == STRING || expr3.type == STRING)
                Error(ERR_IncompatibleExpr);
            else {
                struktura->type = INT_NUMBER;
                if ((struktura->data = malloc(sizeof(int))) == NULL)
                    Error(ERR_AllocFailed);
            }
            TrojAdres(gramatika, expr1.data, expr3.data, struktura.data);
            break;
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
            if (expr1.type == INT_NUMBER && expr3.type == DOUBLE_NUMBER) {
                int docasna_prom = *((int *)expr1.data);
                if ((expr1.data = realloc(expr1.data, sizeof(double))) == NULL)
                    Error(ERR_AllocFailed);
                *expr1.data = docasna_prom;
            }
            if (expr1.type == DOUBLE_NUMBER && expr3.type == INT_NUMBER) {
                int docasna_prom = *((int *)expr3.data);
                if ((expr3.data = realloc(expr3.data, sizeof(double))) == NULL)
                    Error(ERR_AllocFailed);
                *expr3.data = docasna_prom;
            }
            if ((expr1.type == INT_NUMBER && expr3.type == INT_NUMBER) ||
            (expr1.type == DOUBLE_NUMBER && expr3.type == DOUBLE_NUMBER) ||
            (expr1.type == STRING && expr3.type == STRING)) {
                struktura->type = INT_NUMBER;
                if ((struktura->data = malloc(sizeof(int))) == NULL)
                    Error(ERR_AllocFailed);
                TrojAdres(gramatika, expr1.data, expr3.data, struktura.data);
            }
            else return 4;
        break;
        case 11:
            switch (expr1.type) {
                case INT_NUMBER: struktura->type = INT_NUMBER; break;
                case DOUBLE_NUMBER: struktura->type = DOUBLE_NUMBER; break;
                case STRING: struktura->type = STRING; break;
                default: Error(ERR_IncompatibleExpr);
            }
            TrojAdres(gramatika, expr1.data, NULL, struktura.data);
        break;
        case 12:
            switch (expr2.type) {
                case INT_NUMBER: struktura->type = INT_NUMBER); break;
                case DOUBLE_NUMBER: struktura->type = DOUBLE_NUMBER); break;
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
int PrecedencniSA () {
    int val, gramatika;             // promenne
    tExpr* input, suc1, suc2, suc3, top, start;
    tList* Z;                                           // zasobnik
    Init(Z);
    if ((start = malloc(sizeof(tExpr)))== NULL)
        Error(ERR_AllocFailed);
    start->type = SEMICOLON;
    start->terminal = TERMINAL;
    Push(Z, start);
    input = NextToken();                                    // input
    top = FirstTerminal(Z);                             // nacteni pocatku zasobniku
    while (top->type != SEMICOLON && input->type != SEMICOLON) {    // dokud není vstup i zasobnik prazdny
        top = FirstTerminal(Z);                         // nacti dalsi terminal
        switch (precedencni_tabulka[OperatorToIndex(top)][OperatorToIndex(input)])  // najdi pravidlo v prec. tabulce
            case '=':                                   // pokud se rovnaji
                Push(Z, input);                         // zapis do zasobniku
                input = NextToken();                    // nacti dalsi znak na vstupu
                break;
            case '<':                                   // pokud ma zasobnik mensi prioritu nez vstup (dle tabulky)
                tExpr* priority;
                if ((priority = malloc(sizeof(tExpr))) == NULL)
                    Error(ERR_AllocFailed);
                priority->type = 5948;
                Push(Z, priority);                           // zapis do zasobniku znak <
                Push(Z, input);                         // zapis vstup
                input = NextToken();                    // nacti dalsi vstup
                break;
            case '>':                                   // pokud ma zasobnik vetsi prioritu nez vstup (dle tabulky)
                do {
                    suc3 = suc2;                        // zapis 3. predchudce
                    suc2 = suc1;                        // zapis 2. predchudce
                    suc1 = top;                         // zapis prvniho predchudce
                    top = Pop(Z);                       // vytahni prvek na top
                } while (top->type != 5948)             // dokud v promennych nemam cely retezec, pro ktery hledam pravidlo
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
                Error(ERR_SYNTAX);
                break;
    }
    Dispose(Z);                                         // uvolneni zasobniku
}

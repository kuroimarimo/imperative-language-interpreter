#include "prsa.h"

//--------------------------//
//          ERROR           //
//--------------------------//
void Error(int errcode) 
{
    // volani ukonceni
    errorState.state = errcode;
    errorState.line = token.counter_of_lines;
    fatalError(errorState);
}

//--------------------------//
//        NEXT TOKEN        //
//--------------------------//
tExpr* NextToken() 
{
    scanner(srcFile);
    char* str;
    tExpr* struktura;
    static int cond_counter;
    if ((struktura = malloc(sizeof(tExpr))) == NULL)
    {
        Error(ERR_AllocFailed); // chyba
        return NULL;
    }
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
                errorState.state = ERR_NFunction;
                return NULL;
            }
        break;
        case INT_NUMBER: 
            if ((struktura->data = malloc(sizeof(int))) == NULL)
            {
                Error(ERR_AllocFailed);
                return NULL;
            }
            *((int*)struktura->data) = token.int_numb;
        break;
        case DOUBLE_NUMBER:
            if ((struktura->data = malloc(sizeof(double))) == NULL)
            {
                Error(ERR_AllocFailed);
                return NULL;
            }
            *((double*)struktura->data) = token.double_numb;
        break;
        case STRING:
        case IDENTIFIER:
            if ((str = malloc(strlen(token.area)+1)) == NULL)
            {
                Error(ERR_AllocFailed); // chyba
                return NULL;
            }
            struktura->data = str;
            strcpy(struktura->data, token.area);
        break;
    }
    return struktura;
}

//--------------------------//
//      ZASOBNIK            //
//--------------------------//
void Init (tList *Z) 
{                                                               // inicializace
    Z->First = NULL;
}

void Dispose (tList *Z) 
{
    tElemPtr ukazatel = NULL;                                   // deklarace pro predani ukazatele  
    while(Z->First != NULL) 
    {                                                           // cyklus pro free() zasobniku
        ukazatel = Z->First->ptr;
        free(Z->First);
        Z->First = ukazatel;
    }
    Init(Z);                                                    // reinicializace zasobniku
}

tExpr* FirstTerminal (tList *Z) 
{
    tExpr* val = NULL;
    if (Z->First != NULL) 
    {
        val = Z->First->data;
        while (val->terminal == NETERMINAL)                                      // hledam terminal
            if (Z->First->ptr != NULL) 
                val = Z->First->data;                           // aktualni terminal
            else {Error(ERR_SYNTAX);return NULL;}
    }
    return val;
}

void Push (tList *Z, tExpr *val) 
{
    tElemPtr ukazatel = NULL;
    if(Z->First != NULL) ukazatel = Z->First;
    tElemPtr struktura = malloc(sizeof(struct tElem));          // alokace struktury
    if(struktura != NULL) 
    {                                                           // kontrola alokace
        Z->First = struktura;                                   // prirazeni a naplneni
        struktura->data = val;
        struktura->ptr = ukazatel;
    }
    else Error(ERR_AllocFailed);
}

tExpr* Pop (tList *Z) 
{
    tExpr* val = NULL;
    if (Z->First != NULL) 
    {
        val = Z->First->data;                                   // pop hodnoty
        tElemPtr ukazatel = NULL;                               // deklarace pro presun ukazatelu
        if (Z->First->ptr != NULL) ukazatel = Z->First->ptr;    // uchovani ukazatele
        free(Z->First);
        Z->First = ukazatel;                                    // posunuti fronty
    }
    return val;
}

//--------------------------//
//     ISNTRUCTION ARRAY    //
//--------------------------//

/* Dynamically expands the array where the instructions are stored.
 Returns true upon successful completion, else returns false. (realloc failed) */
bool expandInstrList()
{
    int size = instructionList.lenght;
    instructionList.array = realloc(instructionList.array, size * 2 * sizeof(tInstruction));
    return instructionList.array;        // allocation failed = false
}


/* Adds instruction to the global instruction list.
 Returns true upon successful completion, else returns false. (realloc failed) */
bool addInstruction(tInstruction * instr)
{
    if (instructionList.lenght < (instructionList.occupied + 1) && !expandInstrList())
        return false;
    instructionList.array[instructionList.occupied] = * instr;
    ++instructionList.occupied;
    return true;
}


/* Initialises the instruction list.
 Returns true upon successful completion, else returns false. (malloc failed)*/
bool initInstrList()
{
    int size = INITIAL_SIZE;
    if ((instructionList.array = malloc(size * sizeof(tInstruction))) == NULL)
        return false;
    instructionList.lenght = size;
    instructionList.occupied = 0;
    return true;
}

//--------------------------//
//      GRAMATIKA           //
//--------------------------//
int PrecG(tExpr *suc1, tExpr *suc2, tExpr *suc3) 
{
    switch (suc1->terminal) 
    {
        case NETERMINAL:
            if (suc3 == NULL)
            {
                Error(ERR_SYNTAX);
                return 0;
            }
            if (suc3->type == NETERMINAL) 
            {
                if (suc2 == NULL)
                {
                    Error(ERR_SYNTAX);
                    return 0;
                }
                switch (suc2->type) 
                {
                    case PLUS:          return 1;
                    case MINUS:         return 2;
                    case MULTIPLY:      return 3;
                    case DIVIDE:        return 4;
                    case GREATER:       return 5;
                    case GREATER_EQUAL: return 6;
                    case LESS:          return 7;
                    case LESS_EQUAL:    return 8;
                    case EQUAL:         return 9;
                    case NEGATION:      return 10;
                    default: Error(ERR_SYNTAX); return 0;
                }
            }
        break;
        case TERMINAL:
            switch (suc1->type) 
            {
                case STRING:
                case IDENTIFIER:
                case DOUBLE_NUMBER:
                case INT_NUMBER:
                    return 11;
                case L_BRACKET:
                    if (suc2 == NULL || suc3 == NULL) 
                    {
                        Error(ERR_SYNTAX);
                        return 0;
                    }
                    if (suc2->terminal == NETERMINAL)
                        if (suc3->type == R_BRACKET)
                            return 12;
                default: Error(ERR_SYNTAX); return 0;
            }
    }
    Error(ERR_SYNTAX);
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
        default: Error(ERR_ParamType); return; // možná jiný druh erroru
    }
    switch (input1->type) 
    {
        case INT_NUMBER: type = INT_NUMBER; break;
        case DOUBLE_NUMBER: type = DOUBLE_NUMBER; break;
        case STRING: type = STRING; break;
        default: Error(ERR_IncompatibleExpr); return; // možná jiný druh
    }
    tInstruction* instrBeingAdded;
    if ((instrBeingAdded = malloc(sizeof(tInstruction))) == NULL)
    {
        Error(ERR_AllocFailed);
        return;
    }
    instrBeingAdded->operator = operator;
    instrBeingAdded->type = type;
    instrBeingAdded->input1 = input1->data;
    instrBeingAdded->input2 = input2->data;
    instrBeingAdded->output = output->data;
    if((addInstruction(instrBeingAdded)) != true)
    {
        Error(ERR_AllocFailed);
        return;
    }
}

//------------------------------//
//   PREC. SEMANTICKA ANALYZA   //
//------------------------------//
tExpr* SemId(tExpr* identifier, hTab* table) 
{
    hashElem* element;
    element = findElem(table, identifier->data);
    free(identifier->data);
    if (element == NULL) 
    {
        Error(ERR_UndefinedVariable); // nedefinovana prom
        return NULL;
    }
    identifier->data = &element->data.value;
    identifier->type = element->data.type;
    return identifier;
}

tExpr* SemA(tExpr *expr1, tExpr *expr2, tExpr *expr3, int gramatika, hTab *table) 
{
    if (expr1->type == IDENTIFIER)
        SemId(expr1, table);
    if (gramatika != 11) 
    { // BUDE ZMENENO NEEEEEEEEEEEEEEEEEEEEEE!!!!!!!!!!!!!! blbě fujjjj
        if (expr2->type == IDENTIFIER)
            SemId(expr2, table);
        if (expr3->type == IDENTIFIER)
            SemId(expr3, table);
        if ((expr1->type == STRING && (expr3->type == DOUBLE_NUMBER ||
            expr3->type == INT_NUMBER)) || ((expr1->type == DOUBLE_NUMBER ||
            expr1->type == INT_NUMBER) && expr3->type == STRING)) 
        {
            Error(ERR_IncompatibleExpr);
            return NULL;
        }
    }
    tExpr *struktura;
    if ((struktura = malloc(sizeof(tExpr))) == NULL) 
    {
        Error(ERR_AllocFailed); 
        return NULL;
    }
    struktura->terminal = NETERMINAL;
    switch (gramatika) 
    {
        case 1:
        case 2:
        case 3:
        case 4:
            if (expr1->type == DOUBLE_NUMBER || expr3->type == DOUBLE_NUMBER) 
            {
                struktura->type = DOUBLE_NUMBER;
                if ((struktura->data = malloc(sizeof(double))) == NULL) 
                {
                    Error(ERR_AllocFailed); 
                    return NULL;
                }
            }
            else if (expr1->type == STRING || expr3->type == STRING) 
            {
                Error(ERR_IncompatibleExpr); 
                return NULL;
            }
            else 
            {
                struktura->type = INT_NUMBER;
                if ((struktura->data = malloc(sizeof(int))) == NULL) 
                {
                    Error(ERR_AllocFailed); 
                    return NULL;
                }
            }
            TrojAdres(gramatika, expr1->data, expr3->data, struktura->data);
            break;
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
            if (expr1->type == INT_NUMBER && expr3->type == DOUBLE_NUMBER) 
            {
                int docasna_prom = *((int *)expr1->data);
                if ((expr1->data = realloc(expr1->data, sizeof(double))) == NULL) 
                {
                    Error(ERR_AllocFailed); 
                    return NULL;
                }
                *((double*)expr1->data) = docasna_prom;
            }
            if (expr1->type == DOUBLE_NUMBER && expr3->type == INT_NUMBER) 
            {
                int docasna_prom = *((int *)expr3->data);
                if ((expr3->data = realloc(expr3->data, sizeof(double))) == NULL) 
                {
                    Error(ERR_AllocFailed); 
                    return NULL;
                }
                *((double*)expr3->data) = docasna_prom;
            }
            if ((expr1->type == INT_NUMBER && expr3->type == INT_NUMBER) ||
            (expr1->type == DOUBLE_NUMBER && expr3->type == DOUBLE_NUMBER) ||
            (expr1->type == STRING && expr3->type == STRING)) 
            {
                struktura->type = INT_NUMBER;
                if ((struktura->data = malloc(sizeof(int))) == NULL) 
                {
                    Error(ERR_AllocFailed); 
                    return NULL;
                }
                TrojAdres(gramatika, expr1->data, expr3->data, struktura->data);
            }
            else 
            {
                Error(ERR_IncompatibleExpr);
                return NULL;
            }
        break;
        case 11:
            switch (expr1->type) 
            {
                case INT_NUMBER: struktura->type = INT_NUMBER; break;
                case DOUBLE_NUMBER: struktura->type = DOUBLE_NUMBER; break;
                case STRING: struktura->type = STRING; break;
                default: Error(ERR_IncompatibleExpr); return NULL;
            }
            TrojAdres(gramatika, expr1->data, NULL, struktura->data);
        break;
        case 12:
            switch (expr2->type) 
            {
                case INT_NUMBER: struktura->type = INT_NUMBER; break;
                case DOUBLE_NUMBER: struktura->type = DOUBLE_NUMBER; break;
                case STRING: struktura->type = STRING; break;
                default: Error(ERR_IncompatibleExpr); return NULL;
            }
            TrojAdres(gramatika, expr2->data, NULL, struktura->data);
        break;
        default: Error(ERR_IncompatibleExpr); return NULL;
    }
    return struktura;
}

//------------------------------//
//     OPERACE PREC. TABULKY    //
//------------------------------//
int OperatorToIndex (tExpr* op) 
{
    switch (op->type) 
    {
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
        case IDENTIFIER:    return 12;  // i    = 12
        case SEMICOLON:     return 13;  // ;    = 13
        default: Error(ERR_SYNTAX); return -1;
    }
}

//------------------------------//
//  PRECEDENCNI SYNTAX ANALYZA  //
//------------------------------//
void PrecedencniSA (hTab *table) 
{
    int gramatika;             // promenne
    tExpr* input, *suc1, *suc2, *suc3, *top, *start;
    tList* Z;                                           // zasobnik
    if ((Z = malloc(sizeof(tList))) == NULL)
    {
        Error(ERR_AllocFailed);
        return;
    }
    Init(Z);
    if ((start = malloc(sizeof(tExpr))) == NULL)
    {
        Error(ERR_AllocFailed);
        return;
    }
    start->type = SEMICOLON;
    start->terminal = TERMINAL;
    Push(Z, start);
    input = NextToken();                                    // input
    top = FirstTerminal(Z);                             // nacteni pocatku zasobniku
    while (top->type != SEMICOLON && input->type != SEMICOLON)  
    {   // dokud není vstup i zasobnik prazdny
        top = FirstTerminal(Z);                         // nacti dalsi terminal
        tExpr* priority = NULL;
        switch (precedencni_tabulka[OperatorToIndex(top)][OperatorToIndex(input)]) 
        {   // najdi pravidlo v prec. tabulce
            case '=':                                   // pokud se rovnaji
                Push(Z, input);                         // zapis do zasobniku
                input = NextToken();                    // nacti dalsi znak na vstupu
                if (errorState.state == ERR_NCondition) return;
            break;
            case '<':                                   // pokud ma zasobnik mensi prioritu nez vstup (dle tabulky)
                if ((priority = malloc(sizeof(tExpr))) == NULL)
                {
                    Error(ERR_AllocFailed);
                    return;
                }
                priority->type = 5948;
                Push(Z, priority);                           // zapis do zasobniku znak <
                Push(Z, input);                         // zapis vstup
                input = NextToken();                    // nacti dalsi vstup
                if (errorState.state == ERR_NCondition) return;
            break;
            case '>':                                   // pokud ma zasobnik vetsi prioritu nez vstup (dle tabulky)
                do {
                    suc3 = suc2;                        // zapis 3. predchudce
                    suc2 = suc1;                        // zapis 2. predchudce
                    suc1 = top;                         // zapis prvniho predchudce
                    top = Pop(Z);                       // vytahni prvek na top
                } while (top->type != 5948);            // dokud v promennych nemam cely retezec, pro ktery hledam pravidlo
                gramatika = PrecG(suc1, suc2, suc3);
                if (top->type == 5948 && gramatika != 0) 
                { 
                    free(top);
                    top = SemA(suc1, suc2, suc3, gramatika, table);
                    Push(Z, top);   // kontrola na nalezene pravidlo, pokud existuje, prepise na E -> neterminal
                }
                else 
                {
                    Error(ERR_SYNTAX);
                    return;
                }
                free(suc1);
                free(suc2);
                free(suc3);
                suc3 = suc2 = suc1 = NULL;
            break;
            case ' ':                                   // chyba ve vyrazu
                top = input;
                input = NextToken();
                if (input->type == L_BRACKET && top->type == IDENTIFIER) {
                    Dispose(Z);
                    errorState.state = ERR_NFunction;
                    return;
                }
                Error(ERR_SYNTAX);
                return;
            default: Error(ERR_SYNTAX); return;
        }
    }
    Dispose(Z);                                         // uvolneni zasobniku
    errorState.state = ERR_None; 
}

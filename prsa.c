// TODO: add prec. for BOOLOP, 

#include "prsa.h"

//--------------------------//
//          ERROR           //
//--------------------------//
void Error(int errcode) 
{
    errorState.state = errcode;                     // error code
    errorState.line = token.counter_of_lines;       // error occured on line
    fatalError(errorState);                         // exit from error.c
}

//--------------------------//
//        NEXT TOKEN        //
//--------------------------//
tExpr* NextToken() 
{
    scanner(srcFile);                               // scans input file
    char* str;
    tExpr* struktura;                               // struct used to store new token
    static int cond_counter;                        // static counter for condition parse
    if ((struktura = malloc(sizeof(tExpr))) == NULL)
    {
        Error(ERR_AllocFailed);                     // allocation error
        return NULL;
    }
    struktura->terminal = TERMINAL;                 // default
    struktura->type = token.type;                   // sets type of token
    switch (token.type) {
        case L_BRACKET:                             // usage in condition
            //if (errorState.state == ERR_IsFunction) 
                cond_counter++; 
        break;
        case R_BRACKET:                             // usage in condition
            //if (errorState.state == ERR_IsFunction) 
            //{
                cond_counter--; 
                if (cond_counter > 0)                   // if R_BRACKET > L_BRACKET
                {     
                    cond_counter = 0;
                    errorState.state = ERR_NFunction;   // no error, return code of prsa.c
                    return NULL;
                }
            //}
        break;
        case INT_NUMBER: 
            if ((struktura->data = malloc(sizeof(int))) == NULL)
            {
                Error(ERR_AllocFailed);             // allocation failed
                return NULL;
            }
            *((int*)struktura->data) = token.int_numb;      // stores value into data
        break;
        case DOUBLE_NUMBER:
            if ((struktura->data = malloc(sizeof(double))) == NULL)
            {
                Error(ERR_AllocFailed);             // allocation failed
                return NULL;
            }
            *((double*)struktura->data) = token.double_numb;    // stores value into data
        break;
        case STRING:
        case IDENTIFIER:
            if ((str = malloc(strlen(token.area)+1)) == NULL)
            {
                Error(ERR_AllocFailed);             // allocation failed
                return NULL;
            }
            struktura->data = str;                  // defines size of data
            strcpy(struktura->data, token.area);    // stores data from token
        break;
    }
    return struktura;
}

//--------------------------//
//      ZASOBNIK            //
//--------------------------//

/* initialize stack */
void Init (tList *Z) 
{
    Z->First = NULL;
}

/* dispose whole stack in cycle, init default value */
void Dispose (tList *Z) 
{
    tElemPtr ukazatel = NULL; 
    while(Z->First != NULL) 
    {
        ukazatel = Z->First->ptr;
        free(Z->First);
        Z->First = ukazatel;
    }
    Init(Z);
}

/* returns first terminal from stack */
tExpr* FirstTerminal (tList *Z) 
{
    tExpr* val = NULL;
    if (Z->First != NULL) 
    {
        val = Z->First->data;
        while (val->terminal == NETERMINAL)
            if (Z->First->ptr != NULL) 
                val = Z->First->data;
            else 
            {                                           // in case of no terminal in stack
                Error(ERR_SYNTAX);
                return NULL;
            }
    }
    return val;
}

/* stores new tExpr in stack */
void Push (tList *Z, tExpr *val) 
{
    tElemPtr ukazatel = NULL;
    if(Z->First != NULL) ukazatel = Z->First;
    tElemPtr struktura = malloc(sizeof(struct tElem));  // allocation of new tElem
    if(struktura != NULL) 
    {  
        Z->First = struktura;
        struktura->data = val;
        struktura->ptr = ukazatel;
    }
    else Error(ERR_AllocFailed);                        // in case of allocation error
}

/* returns first element from stack*/
tExpr* Pop (tList *Z) 
{
    tExpr* val = NULL;
    if (Z->First != NULL) 
    {
        val = Z->First->data;
        tElemPtr ukazatel = NULL;
        if (Z->First->ptr != NULL) ukazatel = Z->First->ptr;
        free(Z->First);
        Z->First = ukazatel;
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
int PrecG(tExpr *pred3, tExpr *pred2, tExpr *pred1) 
{
    switch (pred1->terminal)                                    // what kind of element is in predecessor1
    {
        case NETERMINAL:                                        // in case of NETERMINAL
            if (pred3 == NULL)                                  // check for predecessor3
            {
                Error(ERR_SYNTAX);                              // in case of empty error
                return 0;
            }
            if (pred3->type == NETERMINAL)                      // if pred3 is also NETERMINAL
            {
                if (pred2 == NULL)                              // check for predecessor2
                {
                    Error(ERR_SYNTAX);                          // in case of empty error
                    return 0;
                }
                switch (pred2->type)                            // what kind of operator is pred2
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
                    default: Error(ERR_SYNTAX); return 0;       // in case of other operator syntax error
                }
            }
        break;
        case TERMINAL:                                          // if predecessor1 is terminal
            switch (pred1->type)                                // what kind of type is predecessor1
            {
                case STRING:
                case IDENTIFIER:
                case DOUBLE_NUMBER:
                case INT_NUMBER:
                    return 11;                                  // in case of string, identifier, double or int returns 11
                case R_BRACKET:                                 // in case of L_BRACKET checks pred2 and pred1 for data else returns error
                    if (pred2 == NULL || pred3 == NULL) 
                    {
                        Error(ERR_SYNTAX);
                        return 0;
                    }
                    if (pred2->terminal == NETERMINAL)          // checks pred2 type (is supposed to by NETERMINAL)
                        if (pred3->type == L_BRACKET)           // checks pred3 for R_BRACKET
                            return 12;
                default: Error(ERR_SYNTAX); return 0;           // in case of no rule returns error
            }
    }
    Error(ERR_SYNTAX);                                          // in case of other possibility returns error
    return 0;
}

//------------------------------//
//        GENEROVANI 3AK        //
//------------------------------//
void TrojAdres(int gramatika, tExpr* input1, tExpr* input2, tExpr* output) 
{
    int operator, type;
    // checks for operator to be stored in TrojAdres
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
        default: Error(ERR_ParamType); return;          // no grammar
    }
    // checks for type of first input, decides what kind of type will be used
    switch (input1->type) 
    {
        case INT_NUMBER: type = INT_NUMBER; break;
        case DOUBLE_NUMBER: type = DOUBLE_NUMBER; break;
        case STRING: type = STRING; break;
        default: Error(ERR_IncompatibleExpr); return;   // not id
    }
    // pushes instruction into array of instructions
    tInstruction* instrBeingAdded;
    if ((instrBeingAdded = malloc(sizeof(tInstruction))) == NULL)
    {
        Error(ERR_AllocFailed);                         // allocation failed
        return;
    }
    instrBeingAdded->operator = operator;
    instrBeingAdded->type = type;
    instrBeingAdded->input1 = input1->data;
    instrBeingAdded->input2 = input2->data;
    instrBeingAdded->output = output->data;
    if((addInstruction(instrBeingAdded)) != true)
    {
        Error(ERR_AllocFailed);                         // allocation failed
        return;
    }
}

//------------------------------//
//   PREC. SEMANTICKA ANALYZA   //
//------------------------------//
tExpr* SemId(tExpr* identifier, hTab* table) 
{
    hashElem* element;
    element = findElem(table, identifier->data);        // find and store identifier from hash table
    free(identifier->data);                             // no more need for identifier name, will use pointer
    if (element == NULL) 
    {
        Error(ERR_UndefinedVariable);                   // error undefined variable
        return NULL;
    }
    identifier->data = &element->data.value;            // stores pointer to data from hash table
    identifier->type = element->data.type;              // stores type of data from hash table
    return identifier;
}

tExpr* SemA(tExpr *expr3, tExpr *expr2, tExpr *expr1, int gramatika, hTab *table) 
{
    if (expr1->type == IDENTIFIER)
        SemId(expr1, table);
    if (gramatika != 11) 
    { // TO DO: update after bool types are added
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
            TrojAdres(gramatika, expr3->data, expr1->data, struktura->data);
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
                TrojAdres(gramatika, expr3->data, expr1->data, struktura->data);
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
/* returns index of element to be used in SA */
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
    int gramatika;                                      // declaration of variables
    tExpr* input, *pred1, *pred2, *pred3, *top, *start;
	//pred1 = pred2 = pred3 = NULL;
    tList* Z;                                           
    if ((Z = malloc(sizeof(tList))) == NULL)            // allocation of stack
    {
        Error(ERR_AllocFailed);                         // in case of allocation error
        return;
    }
    Init(Z);
    if ((start = malloc(sizeof(tExpr))) == NULL)        // allocation of first element
    {
        Error(ERR_AllocFailed);                         // allocation error
        return;
    }
    start->type = SEMICOLON;                            // sets default info for first element
    start->terminal = TERMINAL;
    Push(Z, start);                                     // stores into stack
    input = NextToken();                                // pops first token
    top = FirstTerminal(Z);                             // loads top of stack
    while (top->type != SEMICOLON && input->type != SEMICOLON)  
    {                                                   // until nexttoken and stack are empty
        top = FirstTerminal(Z);                         // loads top of stack
        tExpr* priority = NULL;                         // priority accoring to precedence table
        switch (precedencni_tabulka[OperatorToIndex(top)][OperatorToIndex(input)]) 
        {
            case '=':                                   // in case of equal
                Push(Z, input);                         // push into stack
                input = NextToken();                    // pop next token
                if (errorState.state == ERR_NCondition) return;
            break;
            case '<':                                   // if stack has lower priority than nexttoken
                if ((priority = malloc(sizeof(tExpr))) == NULL)
                {
                    Error(ERR_AllocFailed);             // allocation error
                    return;
                }
                priority->type = 5948;                  // sets priority->type as lower priority
                // ^ possible error
                Push(Z, priority);
                Push(Z, input);
                input = NextToken();
                if (errorState.state == ERR_NCondition) return;
            break;
            case '>':                                   // if stack has higher priority than nexttoken
                do {
                    pred3 = pred2;                      // store 3rd predecessor
                    pred2 = pred1;                      // store 2nd predecessor
                    pred1 = top;                        // store 1st predecessor
                    top = Pop(Z);                       // pop element
                } while (top->type != 5948);            // until lower priority mark is found
                gramatika = PrecG(pred3, pred2, pred1);
                if (top->type == 5948 && gramatika != 0) 
                {                                       // if grammar is found
                    free(top);
                    top = SemA(pred3, pred2, pred1, gramatika, table);
                    Push(Z, top);                       // push NETERMINAL with output pointer
                }
                else 
                {
                    Error(ERR_SYNTAX);
                    return;
                }
                free(pred1);
                free(pred2);
                free(pred3);
                pred3 = pred2 = pred1 = NULL;
            break;
            case ' ':                                   // possible error
                top = input;
                input = NextToken();
                if (input->type == L_BRACKET && top->type == IDENTIFIER) {
                    Dispose(Z);
                    errorState.state = ERR_NFunction;   // not ERROR, found function
                    return;
                }
                Error(ERR_SYNTAX);                      // is error
                return;
            default: Error(ERR_SYNTAX); return;
        }
    }
    Dispose(Z);                                         // frees stack
    errorState.state = ERR_None;                        // no error was found
}

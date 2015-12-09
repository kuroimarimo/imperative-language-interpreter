// TODO: add prec. for BOOLOP, 

#include "prsa.h"
#include "precedence_table.h"
#include "parser.h"

//--------------------------//
//        NEXT TOKEN        //
//--------------------------//
tExpr* NextToken() 
{
    scanner();                               // scans input file
    tExpr* struktura;                               // struct used to store new token
    struktura = customMalloc(sizeof(tExpr));
    struktura->terminal = TERMINAL;                 // default
    struktura->type = token.type;                   // sets type of token
    struktura->data = NULL;
    struktura->identifier = 0;
    switch (token.type) {
        case INT_NUMBER: 
            struktura->data = customMalloc(sizeof(int));
            *((int*)struktura->data) = token.int_numb;          // stores value into data
        break;
        case DOUBLE_NUMBER:
            struktura->data = customMalloc(sizeof(double));
            *((double*)struktura->data) = token.double_numb;    // stores value into data
        break;
        case STRING:
        case IDENTIFIER:
            struktura->data = strDuplicate(token.area);
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

/* returns first terminal from stack */
tExpr* FirstTerminal (tList *Z) 
{
    tElemPtr ptr = Z->First;
    while(ptr != NULL && ptr->data != NULL && ptr->data->terminal == NETERMINAL) {
        ptr = ptr->ptr;
    }

    if(ptr == NULL || ptr->data == NULL) {
//        fatalError(ERR_SYNTAX);
        return NULL;
    }

    return ptr->data;

    tExpr* val = NULL;
    tElemPtr ukazatel = NULL;
    if (Z->First != NULL) 
    {
        val = Z->First->data;
        while (val->terminal == NETERMINAL)
        {
            if (ukazatel != NULL) 
            {
                ukazatel = ukazatel->ptr;
                val = ukazatel->data;
            }
            else 
            {                                                   // in case of no terminal in stack
                fatalError(ERR_SYNTAX);
                return NULL;
            }
        }
    }
    return val;
}

/* stores new tExpr in stack */
void Push (tList *Z, tExpr *val) 
{
    tElemPtr ukazatel = NULL;
    if(Z->First != NULL) ukazatel = Z->First;
    tElemPtr struktura = customMalloc(sizeof(struct tElem));    // allocation of new tElem 
    Z->First = struktura;
    struktura->data = val;
    struktura->ptr = ukazatel;
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
        Z->First = ukazatel;
    }
    return val;
}

void InsertAbove(tList *Z, tExpr* cur, tExpr* new) {
    tElemPtr curPtr = Z->First;
    tElemPtr curPtr2 = NULL;

    while(curPtr != NULL && curPtr->data != cur) {
        curPtr2 = curPtr;
        curPtr = curPtr->ptr;
    }

    if(curPtr2 == NULL) {
        Push(Z, new);
        return;
    }

    if(curPtr != NULL) {
        tElemPtr newPtr = customMalloc(sizeof(struct tElem));
        newPtr->data = new;
        curPtr2->ptr = newPtr;
        newPtr->ptr = curPtr;
    }
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
                fatalError(ERR_SYNTAX);                         // in case of empty error
                return 0;
            }
            if (pred3->terminal == NETERMINAL)                  // if pred3 is also NETERMINAL
            {
                if (pred2 == NULL)                              // check for predecessor2
                {
                    fatalError(ERR_SYNTAX);                     // in case of empty error
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
                    default: fatalError(ERR_SYNTAX); return 0;  // in case of other operator syntax error
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
                case L_BRACKET:                                 // in case of L_BRACKET checks pred2 and pred1 for data else returns error
                    if (pred2 == NULL || pred3 == NULL) 
                    {
                        fatalError(ERR_SYNTAX);
                        return 0;
                    }
                    if (pred2->terminal == NETERMINAL)          // checks pred2 type (is supposed to by NETERMINAL)
                        if (pred3->type == R_BRACKET)           // checks pred3 for R_BRACKET
                            return 12;
                default: fatalError(ERR_SYNTAX); return 0;      // in case of no rule returns error
            }
    }
    return 0;
}

//------------------------------//
//        GENEROVANI 3AK        //
//------------------------------//
tOperand *PomTrojAdres(tExpr* expr)
{
    tOperand *operand = customMalloc(sizeof(tOperand));
    if (expr->identifier == 1)
    {
        switch(expr->type)
        {
            case INT_NUMBER: operand->type = VAR_INT; break;
            case DOUBLE_NUMBER: operand->type = VAR_DOUBLE; break;
            case STRING: operand->type = VAR_STRING; break;
        }
    }
    else   
        operand->type = expr->type;
    operand->operand = expr->data;
    return operand;
}

void TrojAdres(int gramatika, tExpr* input1, tExpr* input2, tExpr* output) 
{
    int operator;
    tOperand *in1, *in2, *out;
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
        case 11:
        case 12: operator = OP_ASSIGN; break;
        default: fatalError(ERR_ParamType); return;          // no grammar
    }
    //
    in1 = PomTrojAdres(input1);
    if (input2 != NULL) in2 = PomTrojAdres(input2);
    out = PomTrojAdres(output);
    // pushes instruction into array of instructions
    tInstruction* instrBeingAdded;
    instrBeingAdded = customMalloc(sizeof(tInstruction));
    instrBeingAdded->operator = operator;
    instrBeingAdded->type = OPERAND;
    instrBeingAdded->input1 = in1;
    if(input2 != NULL)
        instrBeingAdded->input2 = in2;
    else
        instrBeingAdded->input2 = NULL;
    instrBeingAdded->output = out;
    addInstruction(instrBeingAdded);
}

//------------------------------//
//   PREC. SEMANTICKA ANALYZA   //
//------------------------------//
void SemId(tExpr* identifier) 
{
    
    hashElem* element = findVar(identifier->data);
    tVarCoordinates * coordinates;
    coordinates = customMalloc(sizeof(tVarCoordinates));
    coordinates = varToFrame(identifier->data);
    identifier->data = coordinates;
    identifier->identifier = 1;
    switch (element->data.type)
    {
        case VAR_INT:
            identifier->type = INT_NUMBER;
        break;
        case VAR_DOUBLE:
            identifier->type = DOUBLE_NUMBER;
        break;
        case VAR_STRING:
            identifier->type = STRING;
        break;
        default:
            fatalError(ERR_IncompatibleExpr);
            return;
    }
}

tExpr* SemA(tExpr *expr3, tExpr *expr2, tExpr *expr1, int gramatika, hTab *table) 
{
    if (expr1->type == IDENTIFIER)
        SemId(expr1);
    if (gramatika != 11) 
    { // TO DO: update after bool types are added
        if (expr2->type == IDENTIFIER)
            SemId(expr2);
        if (expr3->type == IDENTIFIER)
            SemId(expr3);
        if ((expr1->type == STRING && (expr3->type == DOUBLE_NUMBER ||
            expr3->type == INT_NUMBER)) || ((expr1->type == DOUBLE_NUMBER ||
            expr1->type == INT_NUMBER) && expr3->type == STRING)) 
        {
            fatalError(ERR_IncompatibleExpr);
            return NULL;
        }
    }
    tExpr *struktura;
    struktura = customMalloc(sizeof(tExpr));
    struktura->terminal = NETERMINAL;
    struktura->data = NULL;
    switch (gramatika) 
    {
        case 1:
        case 2:
        case 3:
        case 4:
            if (expr1->type == DOUBLE_NUMBER || expr3->type == DOUBLE_NUMBER) 
            {
                struktura->type = DOUBLE_NUMBER;
                struktura->data = customMalloc(sizeof(double));
            }
            else if (expr1->type == STRING || expr3->type == STRING) 
            {
                fatalError(ERR_IncompatibleExpr); 
                return NULL;
            }
            else 
            {
                struktura->type = INT_NUMBER;
                struktura->data = customMalloc(sizeof(int));
            }
            TrojAdres(gramatika, expr1, expr3, struktura);
            break;
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
            if (expr1->identifier == 0 && expr3->identifier == 0)
            {
                if (expr1->type == INT_NUMBER && expr3->type == DOUBLE_NUMBER) 
                {
                    int docasna_prom = *((int *)expr1->data);
                    expr1->data = customRealloc(expr1->data, sizeof(double));
                    *((double*)expr1->data) = docasna_prom;
                    expr1->type = DOUBLE_NUMBER;
                }
                if (expr1->type == DOUBLE_NUMBER && expr3->type == INT_NUMBER) 
                {
                    int docasna_prom = *((int *)expr3->data);
                    expr3->data = customRealloc(expr3->data, sizeof(double));
                    *((double*)expr3->data) = docasna_prom;
                    expr3->type = DOUBLE_NUMBER;
                }
            }
            if ((expr1->type == INT_NUMBER && expr3->type == INT_NUMBER) ||
            (expr1->type == DOUBLE_NUMBER && expr3->type == DOUBLE_NUMBER) ||
            (expr1->type == STRING && expr3->type == STRING)) 
            {
                struktura->type = INT_NUMBER;
                struktura->data = customMalloc(sizeof(int));
                TrojAdres(gramatika, expr1, expr3, struktura);
            }
            else 
            {
                fatalError(ERR_IncompatibleExpr);
                return NULL;
            }
        break;
        case 11:
            switch (expr1->type) 
            {
                case INT_NUMBER: 
                    struktura->type = INT_NUMBER; 
                    struktura->data = customMalloc(sizeof(int));
                break;
                case DOUBLE_NUMBER: 
                    struktura->type = DOUBLE_NUMBER; 
                    struktura->data = customMalloc(sizeof(double));
                break;
                case STRING: 
                    struktura->type = STRING; 
                    struktura->data = strDuplicate(expr1->data);
                break;
                default: fatalError(ERR_IncompatibleExpr); 
                return NULL;
            }
            TrojAdres(gramatika, expr1, NULL, struktura);
        break;
        case 12:
            switch (expr2->type) 
            {
                case INT_NUMBER: 
                    struktura->type = INT_NUMBER;
                    struktura->data = customMalloc(sizeof(int));
                break;
                case DOUBLE_NUMBER: 
                    struktura->type = DOUBLE_NUMBER; 
                    struktura->data = customMalloc(sizeof(double));
                break;
                case STRING: 
                    struktura->type = STRING; 
                    struktura->data = strDuplicate(expr2->data);
                break;
                default: fatalError(ERR_IncompatibleExpr); return NULL;
            }
            TrojAdres(gramatika, expr2, NULL, struktura);
        break;
        default: fatalError(ERR_IncompatibleExpr); return NULL;
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
        default: fatalError(ERR_SYNTAX); return -1;
    }
}

//------------------------------//
//  PRECEDENCNI SYNTAX ANALYZA  //
//------------------------------//
int PrecedencniSA (hTab *table, int PrecType) 
{
    int gramatika, LeftTerminal, RightTerminal;                                          // declaration of variables
    tExpr* input, *pred1, *pred2, *pred3, *top, *start, *priority, *type;
    input = pred1 = pred2 = pred3 = top = start = type = NULL;
    tList* Z;                                           
    Z = customMalloc(sizeof(tList));
    Init(Z);
    start = customMalloc(sizeof(tExpr));
    start->type = SEMICOLON;
    start->terminal = TERMINAL;
    start->data = NULL;
    Push(Z, start);                                         // stores into stack
    if (PrecType == CALL_CONDITION)                         // no need for L_BRACKET
    {
        input = NextToken();
        if (input->type != L_BRACKET)
        {
            fatalError(ERR_SYNTAX);
            return -1;
        }
    }
    input = NextToken();                                    // pops first token
    if (PrecType == CALL_EXPRESSION)
    {
        LeftTerminal = SEMICOLON;
        RightTerminal = SEMICOLON;
    }
    else
    {
        LeftTerminal = SEMICOLON;
        RightTerminal = R_BRACKET;
    }
    while (1)  
    {      
        top = FirstTerminal(Z);                             // loads top of stack
        if(top->type == LeftTerminal && input->type == RightTerminal)
            break;
        priority = NULL;                                    // priority accoring to precedence table
        switch (precedencni_tabulka[OperatorToIndex(top)][OperatorToIndex(input)]) 
        {
            case '=':                                       // in case of equal
                Push(Z, input);                             // push into stack
                input = NextToken();                        // pop next token
            break;
            case '<':                                       // if stack has lower priority than nexttoken
                priority = customMalloc(sizeof(tExpr));
                priority->type = 5948;                      // sets priority->type as lower priority
                InsertAbove(Z, top, priority);
                Push(Z, input);
                input = NextToken();
            break;
            case '>':                                       // if stack has higher priority than nexttoken
                top = NULL;
                do
                {
                    pred3 = pred2;                          // store 3rd predecessor
                    pred2 = pred1;                          // store 2nd predecessor
                    pred1 = top;                            // store 1st predecessor
                    top = Pop(Z);                           // pop element
                } while (top->type != 5948);                // until lower priority mark is found
                gramatika = PrecG(pred3, pred2, pred1);
                if (top->type == 5948 && gramatika != 0) 
                {                                           // if grammar is found
                    top = SemA(pred3, pred2, pred1, gramatika, table);
                    Push(Z, top);                           // push NETERMINAL with output pointer
                    type = top;
                }
                else 
                {
                    fatalError(ERR_SYNTAX);
                    return -1;
                }
                pred3 = pred2 = pred1 = NULL;
            break;
            case ' ':
                fatalError(ERR_SYNTAX);                     // is error
                return -1;
            default: fatalError(ERR_SYNTAX); return -1;
        }
    }
    return type->type;
}

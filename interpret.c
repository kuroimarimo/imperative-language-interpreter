#include "interpret.h"

void interpret(tInstruction * instruction)
{
    frameStack = frameStackInit(frameStack, 64);		//TODO makro
    instrStack = instrStackInit(64);
    void * output;
    int inputInt;
    double inputDouble;
    tVariable *tempVar, *tempIn1, *tempIn2, *tempOut;
    int i = 0;
    
    while (instruction != NULL)
    {
        //printf("Instrukcia %d\n", i++);
        
        switch (instruction->operator)
        {
            case OP_ASSIGN:
                switch (((tOperand *)instruction->output)->type)
                {
                    case VAR_DOUBLE:
                        tempOut = getVariable(frameStack, (tVarCoordinates *)((tOperand *)instruction->output)->operand);
                        tempOut->value = customMalloc(sizeof(double));
                        switch (((tOperand *)instruction->input1)->type)
                        {
                            case VAR_DOUBLE:
                                tempIn1 = getVariable(frameStack, (tVarCoordinates *)((tOperand *)instruction->input1)->operand);
                                if (!tempIn1->initialized)
                                    fatalError(ERR_UninitVar);
                                
                                *(double *) tempOut->value = *(double *)tempIn1->value;
                                break;
                                
                            case VAR_INT:
                                tempIn1 = getVariable(frameStack, (tVarCoordinates *)((tOperand *)instruction->input1)->operand);
                                if (!tempIn1->initialized)
                                    fatalError(ERR_UninitVar);
                                
                                *(double *)tempOut->value = (double) *(int *)tempIn1->value;
                                break;
                                
                            case DOUBLE_NUMBER:
                                inputDouble = *(double *)((tOperand *)instruction->input1)->operand;
                                *(double *)tempOut->value = inputDouble;
                                break;
                                
                            case INT_NUMBER:
                                inputInt = *(int *)((tOperand *)instruction->input1)->operand;
                                *(double *)tempOut->value = (double) inputInt;
                                break;
                        }
                        tempOut->initialized = true;
                        break;
                        
                    case VAR_INT:
                        tempOut = getVariable(frameStack, (tVarCoordinates *)((tOperand *)instruction->output)->operand);
                        tempOut->value = customMalloc(sizeof(int));
                        switch (((tOperand *)instruction->input1)->type)
                        {
                            case VAR_DOUBLE:
                                tempIn1 = getVariable(frameStack, (tVarCoordinates *)((tOperand *)instruction->input1)->operand);
                                if (!tempIn1->initialized)
                                    fatalError(ERR_UninitVar);
                                
                                *(int *)tempOut->value = (int) *(double *)tempIn1->value;
                                break;
                                
                            case VAR_INT:
                                tempIn1 = getVariable(frameStack, (tVarCoordinates *)((tOperand *)instruction->input1)->operand);
                                if (!tempIn1->initialized)
                                    fatalError(ERR_UninitVar);
                                
                                *(int *)tempOut->value = *(int *)tempIn1->value;
                                break;
                                
                            case DOUBLE_NUMBER:
                                inputDouble = *(double *)((tOperand *)instruction->input1)->operand;
                                *(int *)tempOut->value = (int) inputDouble;
                                break;
                                
                            case INT_NUMBER:
                                inputInt = *(int *)(((tOperand *)instruction->input1)->operand);
                                *(int *)tempOut->value = inputInt;
                                break;
                        }
                        tempOut->initialized = true;
                        break;
                        
                    case INT_NUMBER:
                        output = customMalloc(sizeof(int));
                        ((tOperand *)instruction->output)->operand = output;
                        switch (((tOperand *)instruction->input1)->type)
                        {
                            case VAR_DOUBLE:
                                tempIn1 = getVariable(frameStack, (tVarCoordinates *)((tOperand *)instruction->input1)->operand);
                                if (!tempIn1->initialized)
                                    fatalError(ERR_UninitVar);
                                
                                *(int *)output = (int)*(double *)tempIn1->value;
                                break;
                                
                            case VAR_INT:
                                tempIn1 = getVariable(frameStack, (tVarCoordinates *)((tOperand *)instruction->input1)->operand);
                                if (!tempIn1->initialized)
                                    fatalError(ERR_UninitVar);
                                
                                *(int *)output = *(int *)tempIn1->value;
                                break;
                                
                            case DOUBLE_NUMBER:
                                inputDouble = *(double *)((tOperand *)instruction->input1)->operand;
                                *(int *)output = (int)inputDouble;
                                break;
                                
                            case INT_NUMBER:
                                inputInt = *(int *)((tOperand *)instruction->input1)->operand;
                                *(int *)output = inputInt;
                                break;
                        }
                        break;
                        
                    case DOUBLE_NUMBER:
                        output = customMalloc(sizeof(double));
                        ((tOperand *)instruction->output)->operand = output;
                        switch (((tOperand *)instruction->input1)->type)
                        {
                            case VAR_DOUBLE:
                                tempIn1 = getVariable(frameStack, (tVarCoordinates *)((tOperand *)instruction->input1)->operand);
                                if (!tempIn1->initialized)
                                    fatalError(ERR_UninitVar);
                                
                                *(double *)output = *(double *)tempIn1->value;
                                break;
                                
                            case VAR_INT:
                                tempIn1 = getVariable(frameStack, (tVarCoordinates *)((tOperand *)instruction->input1)->operand);
                                if (!tempIn1->initialized)
                                    fatalError(ERR_UninitVar);
                                
                                *(double *)output = (double)*(int *)tempIn1->value;
                                break;
                                
                            case DOUBLE_NUMBER:
                                inputDouble = *(double *)((tOperand *)instruction->input1)->operand;
                                *(double *)output = inputDouble;
                                break;
                                
                            case INT_NUMBER:
                                inputInt = *(int *)((tOperand *)instruction->input1)->operand;
                                *(double *)output = (double)inputInt;
                                break;
                        }
                        break;
                        
                    case VAR_STRING:
                        tempOut = getVariable(frameStack, (tVarCoordinates *)((tOperand *)instruction->output)->operand);
                        switch (((tOperand *)instruction->input1)->type)
                        {
                            case VAR_STRING:
                                tempIn1 = getVariable(frameStack, (tVarCoordinates *)((tOperand *)instruction->input1)->operand);
                                if (!tempIn1->initialized)
                                    fatalError(ERR_UninitVar);
                                
                                output = strDuplicate(tempIn1->value);
                                break;
                                
                            case STRING:
                                output = strDuplicate(((tOperand *)instruction->input1)->operand);
                                break;
                        }
                        tempOut->value = output;
                        tempOut->initialized = true;
                        break;
                    
                case STRING:
                    switch (((tOperand *)instruction->input1)->type)
                    {
                        case VAR_STRING:
                            tempIn1 = getVariable(frameStack, (tVarCoordinates *)((tOperand *)instruction->input1)->operand);
                            if (!tempIn1->initialized)
                                fatalError(ERR_UninitVar);
                            
                            output = strDuplicate(tempIn1->value);
                            break;
                            
                        case STRING:
                            output = strDuplicate(((tOperand *)instruction->input1)->operand);
                            break;
                    }
                    ((tOperand *)instruction->output)->operand = output;
                    break;
                    
                default:
                    printf("ASSIGN este neimplementovany.\n");
                }
                break;
                
            case OP_SUM:
            case OP_MINUS:
            case OP_MUL:
            case OP_DIV:
            case OP_LT:
            case OP_GT:
            case OP_LE:
            case OP_GE:
            case OP_EQUALS:
            case OP_DIFF:
            case OP_INT:
            case OP_DOUBLE:
            case OP_STRING:
            case OP_COUT:               // zatial fungovalo vzdy
                switch (((tOperand *)instruction->output)->type)
            {
                case INT_NUMBER:
                    printf("%d", *(int *)((tOperand *)instruction->output)->operand);
                    break;
                    
                case DOUBLE_NUMBER:
                    printf("%g", *(double *)((tOperand *)instruction->output)->operand);
                    break;
                    
                case STRING:
                    printf("%s", (char *)((tOperand *)instruction->output)->operand);
                    break;
                    
                case VAR_INT:
                    tempVar = getVariable(frameStack, (tVarCoordinates *)((tOperand *)instruction->output)->operand);
                    if (!tempVar->initialized)
                        fatalError(ERR_UninitVar);
                    
                    printf("%d", *(int *)tempVar->value);
                    break;
                    
                case VAR_DOUBLE:
                    tempVar = getVariable(frameStack, (tVarCoordinates *)((tOperand *)instruction->output)->operand);
                    if (!tempVar->initialized)
                        fatalError(ERR_UninitVar);
                    
                    printf("%g", *(double *)tempVar->value);
                    break;
                    
                case VAR_STRING:
                    tempVar = getVariable(frameStack, (tVarCoordinates *)((tOperand *)instruction->output)->operand);
                    if (!tempVar->initialized)
                        fatalError(ERR_UninitVar);
                    
                    printf("%s", (char *)tempVar->value);
                    break;
                    
            }
                break;
                
            case OP_CIN:			//musi sa spravit inak ako cez scanf, pravdepodobne cez konecny automat OSEKAVA
                tempVar = getVariable(frameStack, (tVarCoordinates *)instruction->output);
                switch (tempVar->type)
            {
                case VAR_INT:
                    if ((scanf("%d", &inputInt) < 0) || !isspace(fgetc(stdin)))
                        fatalError(ERR_ReadInput);
                    
                    tempVar->value = customMalloc(sizeof(int));
                    *(int *)tempVar->value = inputInt;
                    tempVar->initialized = true;
                    break;
                    
                case VAR_DOUBLE:
                    if ((scanf("%lf", &inputDouble) < 0) || !isspace(fgetc(stdin)))
                        fatalError(ERR_ReadInput);
                    
                    printf("Z CIN BOLO NACITANE CISLO %g\n", inputDouble);
                    
                    tempVar->value = customMalloc(sizeof(double));
                    *(double *)tempVar->value = inputDouble;
                    tempVar->initialized = true;
                    break;
                    
                case VAR_STRING:
                    break;
            }
                break;
                
            case OP_GOTO:
                instruction = instruction->input1;
                continue;
                
            case OP_RETURN:         // zaatial iba pre navrat z mainu
                if (instrStackEmpty(instrStack))		//we're in main
                    return;
                //else: store return value (somewhere), pop all frames up to base, jump to *tInstruction on top of the stack
                //               printf("Najskor treba dorobit volanie funkcie.\n");
                //               break;
                
            case OP_CREATE_VAR:
                tempVar = getVariable(frameStack, (tVarCoordinates *)instruction->output);
                tempVar->type = *(int *)instruction->input1;
                break;
                
            case OP_CREATE_FRAME:       // recursion and blocks
                frameCreateAndPush(frameStack, *(unsigned int *)instruction->input1);
                break;
                
            case OP_SET_TOP_TO_BASE:    // nastavenie zakl ramca kvoli returnu a odstranovaniu
                topToBase(frameStack);
                break;
                
            case OP_DISPOSE_FRAME:
                frameStackPop(frameStack);
                break;
                
            case OP_IF_JUMP:
            case OP_IFNOT_JUMP:
            case OP_FUNC_CALL:
            case OP_BUILT_IN:
            case OP_GET_RETURN_VALUE:
                
            case OP_NOP:            // <3
                break;
                
            default:
                printf("NEIMPLEMENTOVANA INSTRUKCIA.\n");
                
        }
        
        instruction = instruction->next;
    }
    
    fatalError(ERR_OtherRunningErr);
}


#include "interpret.h"

#define INIT_INPUT_SIZE 16
#define OPERATION(a,b,x) (a) x (b)

enum oper {ADD, SUBSTR, MUL, DIV};

/* Reads a string terminated by white space character from input.
   Returns pointer to the string. */
char * inputString()
{
    int size = INIT_INPUT_SIZE;
    int len = 0;
    int c;
    

    
    char * str = customMalloc(size * sizeof(char));   // alloc for initial size
    
    while(!isspace(c = getchar()))
    {
        str[len++] = c;
        if(len == size)
            str = customRealloc(str, (size *= 2) * sizeof(char));
    }
    
    str[len++]='\0';
    
    return customRealloc(str, len * sizeof(char));
}

void interpret(tInstruction * instruction)              // TODO frameStack, instrStack UNGLOBALIZE
{
    frameStack = frameStackInit(frameStack, 64);		//TODO makro
    instrStack = instrStackInit(64);
    int inputInt;
    double inputDouble;
    tVariable *tempVar, *tempIn1, *tempIn2, *tempOut;
	char * inputStr;
	enum oper operator;
    //int i = 0;
    
    while (instruction != NULL)
    {
        //printf("Instrukcia %d\n", i++);
        
        switch (instruction->operator)
        {
            case OP_ASSIGN:
				tempOut = getVariable(frameStack, instruction->output);
				tempIn1 = getVariable(frameStack, instruction->input1);
                
                if (!tempIn1->initialized)
                    fatalError(ERR_UninitVar);
				
				switch (tempOut->type)
				{
					case VAR_INT:
						if (!tempOut->initialized)
							tempOut->value = customMalloc(sizeof(int));
						switch (tempIn1->type)
						{
							case VAR_INT:
								*(int *)tempOut->value = *(int *)tempIn1->value;
								break;

							case VAR_DOUBLE:
								*(int *)tempOut->value = (int) *(double *)tempIn1->value;
								break;
						}
						break;


					case VAR_DOUBLE:
						if (!tempOut->initialized)
							tempOut->value = customMalloc(sizeof(double));
						switch (tempIn1->type)
						{
						case VAR_INT:
                            *(double *)tempOut->value = (double) *(int *)tempIn1->value;
							break;

						case VAR_DOUBLE:
							*(double *)tempOut->value = *(double *)tempIn1->value;
							break;
						}
						break;

					case VAR_STRING:
						tempOut->value = strDuplicate(tempIn1->value);
						break;
				}
				tempOut->initialized = true;
				break;

                
            case OP_SUM:
                operator = ADD;
                
            case OP_MINUS:
                if (instruction->operator == OP_MINUS)
                    operator = SUBSTR;
                
            case OP_MUL:
                if (instruction->operator == OP_MUL)
                    operator = MUL;
                
            case OP_DIV:                                                    // TO DO: napisat normalne, osetrit delenie nulou
                if (instruction->operator == OP_MUL)
                    operator = DIV;
                
                tempOut = getVariable(frameStack, instruction->output);
                tempIn1 = getVariable(frameStack, instruction->input1);
                tempIn2 = getVariable(frameStack, instruction->input2);
                
                if (!tempIn1->initialized || !tempIn2->initialized)
                    fatalError(ERR_UninitVar);
                
                switch (tempOut->type)
                {
                    case VAR_INT:
                        if (!tempOut->initialized)
                            tempOut->value = customMalloc(sizeof(int));
                        switch (operator)
                        {
                            case ADD:
                                *(int *)tempOut->value = *(int *)tempIn1->value + *(int *)tempIn2->value;
                                break;
                        
                            case SUBSTR:
                                *(int *)tempOut->value = *(int *)tempIn1->value - *(int *)tempIn2->value;
                                break;
                                
                            case MUL:
                                *(int *)tempOut->value = *(int *)tempIn1->value * *(int *)tempIn2->value;
                                break;
                                
                            case DIV:
                                *(int *)tempOut->value = *(int *)tempIn1->value / *(int *)tempIn2->value;
                                break;
                        }
                        break;
                        
                    case VAR_DOUBLE:
                        if (!tempOut->initialized)
                            tempOut->value = customMalloc(sizeof(double));
                        switch (operator)
                        {
                            case ADD:
                                *(double *)tempOut->value = *(double *)tempIn1->value + *(double *)tempIn2->value;
                                break;
                                
                            case SUBSTR:
                                *(double *)tempOut->value = *(double *)tempIn1->value - *(double *)tempIn2->value;
                                break;
                                
                            case MUL:
                                *(double *)tempOut->value = *(double *)tempIn1->value * *(double *)tempIn2->value;
                                break;
                                
                            case DIV:
                                *(double *)tempOut->value = *(double *)tempIn1->value / *(double *)tempIn2->value;
                                break;
                        }
                        break;
                }
                break;
				
            case OP_LT:
            case OP_GT:
            case OP_LE:
            case OP_GE:
            case OP_EQUALS:
            case OP_DIFF:
            case OP_INT:
            case OP_DOUBLE:
            case OP_STRING:
				printf("Neimplementovana instrukcia.\n");
				break;
            case OP_COUT:               // zatial fungovalo vzdy
				tempOut = getVariable(frameStack, instruction->output);
                switch (tempOut->type)
				{
					/*case INT_NUMBER:
						printf("%d", *(int *)((tOperand *)instruction->output)->operand);
						break;
                    
					case DOUBLE_NUMBER:
						printf("%g", *(double *)((tOperand *)instruction->output)->operand);
						break;
                    
					case STRING:
						printf("%s", (char *)((tOperand *)instruction->output)->operand);
						break;*/
                    
					case VAR_INT:
						if (!tempOut->initialized)
							fatalError(ERR_UninitVar);
                    
						printf("%d", *(int *)tempOut->value);
						break;
                    
					case VAR_DOUBLE:
						if (!tempOut->initialized)
							fatalError(ERR_UninitVar);

						printf("%g", *(double *)tempOut->value);
						break;
                    
					case VAR_STRING:
						if (!tempOut->initialized)
							fatalError(ERR_UninitVar);

						printf("%s", (char *)tempOut->value);
						break;
                    
				}
                break;
                
            case OP_CIN:
                tempVar = getVariable(frameStack, (tVarCoordinates *)instruction->output);
                switch (tempVar->type)
				{
					case VAR_INT:
						if ((scanf("%d", &inputInt) < 0) || !isspace(fgetc(stdin)))
							fatalError(ERR_ReadInput);
                    
						if (!tempVar->initialized)
							tempVar->value = customMalloc(sizeof(int));
						*(int *)tempVar->value = inputInt;
						tempVar->initialized = true;
						break;
                    
					case VAR_DOUBLE:
						if ((scanf("%lf", &inputDouble) < 0) || !isspace(fgetc(stdin)))
							fatalError(ERR_ReadInput);
                    
						if (!tempVar->initialized)
							tempVar->value = customMalloc(sizeof(double));
						*(double *)tempVar->value = inputDouble;
						tempVar->initialized = true;
						break;
                    
					case VAR_STRING:
                        inputStr = inputString();
                        if (!tempVar->initialized)
                            tempVar->value = customMalloc(sizeof(int));
                        tempVar->value = inputStr;
                        tempVar->initialized = true;
						break;
				}
                break;
                
            case OP_GOTO:
                instruction = instruction->input1;
                continue;
                
            case OP_RETURN:         // zaatial iba pre navrat z mainu
                if (instrStackEmpty(instrStack))		//we're in main
                    return;
				//nastavenie hodnoty TODO
				frameStackPopUntilBase(frameStack);
				instruction = instrStackPop(instrStack);
				continue;
                //else: store return value (somewhere), pop all frames up to base, jump to *tInstruction on top of the stack
                //               printf("Najskor treba dorobit volanie funkcie.\n");
                //               break;
                
            case OP_CREATE_VAR:
                tempVar = getVariable(frameStack, instruction->output);
                tempVar->type = *(int *)instruction->input1;
                break;
                
            case OP_CREATE_FRAME:       // recursion and blocks
				//printf("Velkost zakladneho ramca v instrukcii: %d. \n", (unsigned int) *findElem(globalST, "fififi")->data.baseFrameSize);
				//int a = (signed int) *(int *)instruction->input1;
				//printf("_______________________%d______________________\n", a);
                frameCreateAndPush(frameStack, *(int *)instruction->input1);
                break;
                
            case OP_SET_TOP_TO_BASE:    // nastavenie zakl ramca kvoli returnu a odstranovaniu
                topToBase(frameStack);
                break;
                
            case OP_DISPOSE_FRAME:
                frameStackPop(frameStack);
                break;
                
			case OP_IF_JUMP:
				tempIn1 = getVariable(frameStack, instruction->input1);
				if (*(int *)tempIn1->value)
				{
					instruction = instruction->output;
					continue;
				}
				break;

            case OP_IFNOT_JUMP:
				tempIn1 = getVariable(frameStack, instruction->input1);
				if (!*(int *)tempIn1->value)
				{
					instruction = instruction->output;
					continue;
				}
				break;

            case OP_FUNC_CALL:
				instrStackPush(instrStack, instruction->next);
				instruction = ((tInstrList *)instruction->input1)->first;
				continue;

            case OP_BUILT_IN:
            case OP_GET_RETURN_VALUE:
				printf("ASSIGN este neimplementovany.\n");
				break;
                
            case OP_NOP:            // <3
                break;

			case OP_SET_CONSTANT:
				tempOut = getVariable(frameStack, instruction->output);
				tempOut->value = instruction->input1;
				tempOut->initialized = true;
				break;
                
            default:
                printf("NEIMPLEMENTOVANA INSTRUKCIA.\n");
                
        }
        
        instruction = instruction->next;
    }
    
    fatalError(ERR_OtherRunningErr);
}


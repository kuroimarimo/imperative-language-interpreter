#include "interpret.h"

#define INIT_INPUT_SIZE 16
#define INIT_FRAMESTACK 64

#define OPERATION(a,b,x) (a) x (b)
#define stringify(x) #x

/* Reads a string terminated by white space character from input.
   Returns pointer to the string. */
char * inputString()
{
    int size = INIT_INPUT_SIZE;
    int len = 0;
    int c;
    
    char * str = customMalloc(size * sizeof(char));         // allocates for initial size
    
    while(!isspace(c = getchar()))
    {
        str[len++] = c;
        if(len == size)
            str = customRealloc(str, (size *= 2) * sizeof(char));
    }
    
    str[len++]='\0';
    return customRealloc(str, len * sizeof(char));
}

void interpret(tInstruction * instruction)                  // TODO frameStack, instrStack UNGLOBALIZE
{
    frameStack = frameStackInit(frameStack, INIT_FRAMESTACK);
    instrStack = instrStackInit(INIT_FRAMESTACK);
    int inputInt;
    double inputDouble;
    tVariable *tempVar, *tempIn1, *tempIn2, *tempOut;
    //	char * inputStr;

    //int i = 0;
    
    /*const char* instrOp[] =                               // debugging purposes only
    {
        stringify(OP_ASSIGN),
        stringify(OP_SUM),
        stringify(OP_MINUS),
        stringify(OP_MUL),
        stringify(OP_DIV),
        stringify(OP_MOD),
        stringify(OP_LT),
        stringify(OP_GT),
        stringify(OP_LE),
        stringify(OP_GE),
        stringify(OP_EQUALS),
        stringify(OP_DIFF),
        stringify(OP_COUT),
        stringify(OP_CIN),
        stringify(OP_IF),
        stringify(OP_ELSE),
        stringify(OP_LABEL),
        stringify(OP_GOTO),
        stringify(OP_RETURN),
        stringify(OP_SET_CONSTANT),
        stringify(OP_CREATE_VAR),
        stringify(OP_CREATE_FRAME),
        stringify(OP_SET_TOP_TO_BASE),
        stringify(OP_DISPOSE_FRAME),
        stringify(OP_IF_JUMP),
        stringify(OP_IFNOT_JUMP),
        stringify(OP_FUNC_CALL),
        stringify(OP_BUILT_IN),
        stringify(OP_GET_RETURN_VALUE),
        stringify(OP_NOP)
    };*/

    
    while (instruction != NULL)
    {
        //printf("Instrukcia %d.\n", i++);
        
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
						switch (tempIn1->type)
						{
							case VAR_INT:
								tempOut->value.i = tempIn1->value.i;
                                tempOut->initialized = true;
                                printf("out: %d in1: %d\n", tempOut->value.i, tempIn1->value.i);
								break;

							case VAR_DOUBLE:
								tempOut->value.i = (int) tempIn1->value.d;
                                tempOut->initialized = true;
								break;
						}
						break;


					case VAR_DOUBLE:
						switch (tempIn1->type)
						{
						case VAR_INT:
                            tempOut->value.d = (double) tempIn1->value.i;
                            tempOut->initialized = true;
							break;

						case VAR_DOUBLE:
							tempOut->value.d = tempIn1->value.d;
                            tempOut->initialized = true;
							break;
						}
						break;

					case VAR_STRING:
						tempOut->value.s = strDuplicate(tempIn1->value.s);
						break;
				}
				tempOut->initialized = true;
				break;

                
            case OP_SUM:
                tempOut = getVariable(frameStack, instruction->output);
                tempIn1 = getVariable(frameStack, instruction->input1);
                tempIn2 = getVariable(frameStack, instruction->input2);
                
                if (!tempIn1->initialized || !tempIn2->initialized)
                    fatalError(ERR_UninitVar);
                
                switch (tempOut->type)
                {
                    case VAR_INT:
                        tempOut->value.i = tempIn1->value.i + tempIn2->value.i;
                        tempOut->initialized = true;
                        break;
                        
                    case VAR_DOUBLE:
                        tempOut->value.d = tempIn1->value.d + tempIn2->value.d;
                        tempOut->initialized = true;
                        break;
                }
                break;
                
            case OP_MINUS:
                tempOut = getVariable(frameStack, instruction->output);
                tempIn1 = getVariable(frameStack, instruction->input1);
                tempIn2 = getVariable(frameStack, instruction->input2);
                
                if (!tempIn1->initialized || !tempIn2->initialized)
                    fatalError(ERR_UninitVar);
                
                switch (tempOut->type)
                {
                    case VAR_INT:
                        tempOut->value.i = tempIn1->value.i - tempIn2->value.i;
                        tempOut->initialized = true;
                        break;
                        
                    case VAR_DOUBLE:
                        tempOut->value.d = tempIn1->value.d - tempIn2->value.d;
                        tempOut->initialized = true;
                        break;
                }
                break;
                
            case OP_MUL:
                tempOut = getVariable(frameStack, instruction->output);
                tempIn1 = getVariable(frameStack, instruction->input1);
                tempIn2 = getVariable(frameStack, instruction->input2);
                
                if (!tempIn1->initialized || !tempIn2->initialized)
                    fatalError(ERR_UninitVar);
                
                switch (tempOut->type)
                {
                    case VAR_INT:
                        tempOut->value.i = tempIn1->value.i * tempIn2->value.i;
                        tempOut->initialized = true;
                        break;
                        
                    case VAR_DOUBLE:
                        tempOut->value.d = tempIn1->value.d * tempIn2->value.d;
                        tempOut->initialized = true;
                        break;
                }
                break;
                
            case OP_DIV:
                tempOut = getVariable(frameStack, instruction->output);
                tempIn1 = getVariable(frameStack, instruction->input1);
                tempIn2 = getVariable(frameStack, instruction->input2);
                
                if (!tempIn1->initialized || !tempIn2->initialized)
                    fatalError(ERR_UninitVar);
                
                if (!tempIn2->value.i || !tempIn2->value.d)
                    fatalError(ERR_ZeroDivision);
                
                switch (tempOut->type)
                {
                    case VAR_INT:
                        tempOut->value.i = tempIn1->value.i / tempIn2->value.i;
                        tempOut->initialized = true;
                        break;
                        
                    case VAR_DOUBLE:
                        tempOut->value.d = tempIn1->value.d / tempIn2->value.d;
                        tempOut->initialized = true;
                        break;
                }
                break;
				
            case OP_LT:
                tempOut = getVariable(frameStack, instruction->output);
                tempIn1 = getVariable(frameStack, instruction->input1);
                tempIn2 = getVariable(frameStack, instruction->input2);
                
                if (!tempIn1->initialized || !tempIn2->initialized)
                    fatalError(ERR_UninitVar);
                
                if (tempIn1->type == VAR_INT && tempIn2->type == VAR_INT)                   // int < int
                    tempOut->value.i = (tempIn1->value.i < tempIn2->value.i);
                else if (tempIn1->type == VAR_INT && tempIn2->type == VAR_DOUBLE)           // int < double
                    tempOut->value.i = (tempIn1->value.i < tempIn2->value.d);
                else if (tempIn1->type == VAR_DOUBLE && tempIn2->type == VAR_INT)           // double < int
                    tempOut->value.i = (tempIn1->value.d < tempIn2->value.i);
                else if (tempIn1->type == VAR_DOUBLE && tempIn2->type == VAR_DOUBLE)        // double < double
                    tempOut->value.i = (tempIn1->value.d < tempIn2->value.d);
                else
                    tempOut->value.i = strcmp(tempIn1->value.s, tempIn2->value.s) < 0? true : false;    // string < string
                
                tempOut->initialized = true;
                break;
                
            case OP_GT:
                tempOut = getVariable(frameStack, instruction->output);
                tempIn1 = getVariable(frameStack, instruction->input1);
                tempIn2 = getVariable(frameStack, instruction->input2);
                
                if (!tempIn1->initialized || !tempIn2->initialized)
                    fatalError(ERR_UninitVar);
                
                if (tempIn1->type == VAR_INT && tempIn2->type == VAR_INT)                   // int > int
                    tempOut->value.i = (tempIn1->value.i > tempIn2->value.i);
                else if (tempIn1->type == VAR_INT && tempIn2->type == VAR_DOUBLE)           // int > double
                    tempOut->value.i = (tempIn1->value.i > tempIn2->value.d);
                else if (tempIn1->type == VAR_DOUBLE && tempIn2->type == VAR_INT)           // double > int
                    tempOut->value.i = (tempIn1->value.d > tempIn2->value.i);
                else if (tempIn1->type == VAR_DOUBLE && tempIn2->type == VAR_DOUBLE)        // double > double
                    tempOut->value.i = (tempIn1->value.d > tempIn2->value.d);
                else
                    tempOut->value.i = strcmp(tempIn1->value.s, tempIn2->value.s) > 0? true : false;    // string > string
                
                tempOut->initialized = true;
                break;
                
            case OP_LE:
                tempOut = getVariable(frameStack, instruction->output);
                tempIn1 = getVariable(frameStack, instruction->input1);
                tempIn2 = getVariable(frameStack, instruction->input2);
                
                if (!tempIn1->initialized || !tempIn2->initialized)
                    fatalError(ERR_UninitVar);
                
                if (tempIn1->type == VAR_INT && tempIn2->type == VAR_INT)                   // int <= int
                    tempOut->value.i = (tempIn1->value.i <= tempIn2->value.i);
                else if (tempIn1->type == VAR_INT && tempIn2->type == VAR_DOUBLE)           // int <= double
                    tempOut->value.i = (tempIn1->value.i <= tempIn2->value.d);
                else if (tempIn1->type == VAR_DOUBLE && tempIn2->type == VAR_INT)           // double <= int
                    tempOut->value.i = (tempIn1->value.d <= tempIn2->value.i);
                else if (tempIn1->type == VAR_DOUBLE && tempIn2->type == VAR_DOUBLE)        // double <= double
                    tempOut->value.i = (tempIn1->value.d <= tempIn2->value.d);
                else
                    tempOut->value.i = strcmp(tempIn1->value.s, tempIn2->value.s) <= 0? true : false;   // string <= string
            
                tempOut->initialized = true;
                break;
                
            case OP_GE:
                tempOut = getVariable(frameStack, instruction->output);
                tempIn1 = getVariable(frameStack, instruction->input1);
                tempIn2 = getVariable(frameStack, instruction->input2);
                
                if (!tempIn1->initialized || !tempIn2->initialized)
                    fatalError(ERR_UninitVar);
                
                if (tempIn1->type == VAR_INT && tempIn2->type == VAR_INT)                   // int >= int
                    tempOut->value.i = (tempIn1->value.i >= tempIn2->value.i);
                else if (tempIn1->type == VAR_INT && tempIn2->type == VAR_DOUBLE)           // int >= double
                    tempOut->value.i = (tempIn1->value.i >= tempIn2->value.d);
                else if (tempIn1->type == VAR_DOUBLE && tempIn2->type == VAR_INT)           // double >= int
                    tempOut->value.i = (tempIn1->value.d >= tempIn2->value.i);
                else if (tempIn1->type == VAR_DOUBLE && tempIn2->type == VAR_DOUBLE)        // double >= double
                    tempOut->value.i = (tempIn1->value.d >= tempIn2->value.d);
                else
                    tempOut->value.i = strcmp(tempIn1->value.s, tempIn2->value.s) >= 0? true : false;   // string >= string
                
                tempOut->initialized = true;
                break;
                
            case OP_EQUALS:
                tempOut = getVariable(frameStack, instruction->output);
                tempIn1 = getVariable(frameStack, instruction->input1);
                tempIn2 = getVariable(frameStack, instruction->input2);
                
                if (!tempIn1->initialized || !tempIn2->initialized)
                    fatalError(ERR_UninitVar);
                
                if (tempIn1->type == VAR_INT && tempIn2->type == VAR_INT)                   // int == int
                    tempOut->value.i = (tempIn1->value.i == tempIn2->value.i);
                else if (tempIn1->type == VAR_INT && tempIn2->type == VAR_DOUBLE)           // int == double
                    tempOut->value.i = (tempIn1->value.i == tempIn2->value.d);
                else if (tempIn1->type == VAR_DOUBLE && tempIn2->type == VAR_INT)           // double == int
                    tempOut->value.i = (tempIn1->value.d == tempIn2->value.i);
                else if (tempIn1->type == VAR_DOUBLE && tempIn2->type == VAR_DOUBLE)        // double == double
                    tempOut->value.i = (tempIn1->value.d == tempIn2->value.d);
                else
                    tempOut->value.i = !strcmp(tempIn1->value.s, tempIn2->value.s);         // string == string
                
                tempOut->initialized = true;
                break;
                
            case OP_DIFF:
                tempOut = getVariable(frameStack, instruction->output);
                tempIn1 = getVariable(frameStack, instruction->input1);
                tempIn2 = getVariable(frameStack, instruction->input2);
                
                if (!tempIn1->initialized || !tempIn2->initialized)
                    fatalError(ERR_UninitVar);
                
                if (tempIn1->type == VAR_INT && tempIn2->type == VAR_INT)                   // int != int
                    tempOut->value.i = (tempIn1->value.i != tempIn2->value.i);
                else if (tempIn1->type == VAR_INT && tempIn2->type == VAR_DOUBLE)           // int != double
                    tempOut->value.i = (tempIn1->value.i != tempIn2->value.d);
                else if (tempIn1->type == VAR_DOUBLE && tempIn2->type == VAR_INT)           // double != int
                    tempOut->value.i = (tempIn1->value.d != tempIn2->value.i);
                else if (tempIn1->type == VAR_DOUBLE && tempIn2->type == VAR_DOUBLE)        // double != double
                    tempOut->value.i = (tempIn1->value.d != tempIn2->value.d);
                else
                    tempOut->value.i = strcmp(tempIn1->value.s, tempIn2->value.s);          // string != string
                
                tempOut->initialized = true;
                break;
                
            case OP_COUT:
				tempOut = getVariable(frameStack, instruction->output);
                switch (tempOut->type)
				{
					case VAR_INT:
						if (!tempOut->initialized)
							fatalError(ERR_UninitVar);
                    
						printf("%d", tempOut->value.i);
						break;
                    
					case VAR_DOUBLE:
						if (!tempOut->initialized)
							fatalError(ERR_UninitVar);

						printf("%g", tempOut->value.d);
						break;
                    
					case VAR_STRING:
						if (!tempOut->initialized)
							fatalError(ERR_UninitVar);

						printf("%s", tempOut->value.s);
						break;
                    
				}
                break;
                
            case OP_CIN:
                tempVar = getVariable(frameStack, (tVarCoordinates *)instruction->output);
                switch (tempVar->type)
                {
                    case VAR_INT:
                        while (isspace(inputInt = getchar()))   // cleans the input off spaces
                            if (inputInt == '\n' || !isdigit(inputInt))     // throws an error if there's no or non-numerical input
                                fatalError(ERR_ReadInput);
                        ungetc(inputInt, stdin);                // returns the last loaded char (not white char) to stdin
                        
                        if (scanf("%d", &inputInt) < 0)
                            fatalError(ERR_ReadInput);
                        printf("inputInt: %d\n", inputInt);
                        tempVar->value.i = inputInt;
                        tempVar->initialized = true;
                        
                        if ((inputInt = getchar()) != '\n')     // cleans the stdin off '\n'
                            ungetc(inputInt, stdin);
                        break;
                        
                    case VAR_DOUBLE:
                        while (isspace(inputInt = getchar()))   // cleans the input off spaces
                            if (inputInt == '\n' || !isdigit(inputInt))     // throws an error if there's no or non-numerical input
                                fatalError(ERR_ReadInput);
                        ungetc(inputInt, stdin);                // returns the last loaded char (not white char) to stdin

                        if (scanf("%lf", &inputDouble) < 0)
                            fatalError(ERR_ReadInput);
                        tempVar->value.d = inputDouble;
                        tempVar->initialized = true;
                        
                        if ((inputInt = getchar()) != '\n')     // cleans stdin off '\n'
                            ungetc(inputInt, stdin);
                        break;
                        
                    case VAR_STRING:
                        tempVar->value.s = inputString();
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
                //printf("int: %d double: %lf string: %s\n", *(int *)instruction->input1, *(double *)instruction->input1, instruction->input1);
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
				if (tempIn1->value.i)
				{
					instruction = instruction->output;
					continue;
				}
				break;

            case OP_IFNOT_JUMP:
				tempIn1 = getVariable(frameStack, instruction->input1);
				if (!tempIn1->value.i)
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
				printf("OP_GET_RETURN_VALUE este neimplementovana.\n");
				break;
                
            case OP_NOP:            // <3
                break;

			case OP_SET_CONSTANT:
				tempOut = getVariable(frameStack, instruction->output);
                switch (tempOut->type)
                {
                    case VAR_INT:
                        tempOut->value.i = *(int *) instruction->input1;
                        break;
                        
                    case VAR_DOUBLE:
                        tempOut->value.d = *(double *) instruction->input1;
                        break;
                        
                    case VAR_STRING:
                        tempOut->value.s = instruction->input1;
                        break;
                }
				tempOut->initialized = true;
				break;
                
            default:
                printf("NEIMPLEMENTOVANA INSTRUKCIA.\n");
        }
        
        //printf("Spravil som %s.\n\n",instrOp[instruction->operator]);
        
        instruction = instruction->next;
    }
    
    fatalError(ERR_OtherRunningErr);
}

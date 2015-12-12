#include "parser.h"

int isKeyword(int tokenType)
{
    return (tokenType >= K_CIN) && (tokenType <= K_DO);
}

void parse()
{
    hashElem activeElem;							//informations about curently parsed function
    activeElem.key = NULL;

    globalST = hTabInit(INIT_ST_SIZE);

    rule_funcdef(&activeElem);						//process function definition/declaration
    
	tableStackDispose(localSTstack);

	if (!isFunct(F_MAIN) || (findElem(globalST, F_MAIN)->data.state != DEFINED))				//there's no main
		fatalError(ERR_UndefinedFunction);

	checkFuncDefinitions();

	interpret(findElem(globalST, F_MAIN)->data.instructions->first);
}

// rule:    <prog> -> type id <param-list> <func-defined>   || EOF
void rule_funcdef(hashElem * activeElem)
{
    hashElemInit(activeElem);
	localSTstack = tableStackInit(INIT_ST_SIZE);

    scanner();
    if (token.type == EOF)				//the whole file has been processed
        return;

	switch (token.type)					//get function type
	{
		case K_INT:
		case K_DOUBLE:
		case K_STRING:
			functionType = getFuncType(token.type);
			break;

		default:
			fatalError(ERR_SYNTAX);
	}


    activeElem->data.type = functionType;				//save the function type
    activeElem->data.state = DECLARED;
	
	initInstrList();

    scanner();
    if (token.type != IDENTIFIER)
       fatalError(ERR_SYNTAX);

	activeElem->key = strDuplicate(token.area);			//save the function name
    
    rule_paramList(activeElem);							//process parameters

	//main should be of type int and with no parameters

    if (!strcmp(activeElem->key, F_MAIN) && ((activeElem->data.numberOfParams != 0) || (activeElem->data.type != FUNC_INT)))
        fatalError(ERR_UndefinedFunction);

    rule_funcDefined(activeElem);						//process the rest of definition/declaration

	tableStackDispose(localSTstack);
    rule_funcdef(activeElem);
}

//rule:     <func-defined> -> { <st-list>        ||      ; 
void rule_funcDefined(hashElem * activeElem)
{
    hashElem * temp = findElem(globalST, activeElem->key);

    if (temp == NULL)	
		temp = addElem(globalST, activeElem->key, &activeElem->data);		//there's no such function declared in symbol table, create it then

    else if (!compareSymbol(temp, activeElem))								// the function has already been declared
        fatalError(ERR_AttemptedRedefFunction);

    scanner();
	switch (token.type)
	{
		case SEMICOLON:								//only a declaration, nothing more to be done here
			tableStackPop(localSTstack);
			return;

		case L_CURLY_BRACKET:						//function definition, continue
			break;

		default: 
			fatalError(ERR_SYNTAX);
	}

    if (temp->data.state == DEFINED)		
        fatalError(ERR_AttemptedRedefFunction);		//function is already defined

	temp->data.state = DEFINED;
	temp->data.instructions = instructionList;		// assign the current instruction list
	temp->data.baseFrameSize = &tableStackTop(localSTstack)->numStoredElem;

	if (!strcmp(temp->key, F_MAIN))
		generateInstruction(OP_CREATE_FRAME, &tableStackTop(localSTstack)->numStoredElem, NULL, NULL);	//main is called first, it creates it's own base frame
																										//other functions will have it created before calling them and filled with parameters
	
	generateInstruction(OP_SET_TOP_TO_BASE, NULL, NULL, NULL);	//set the top frame to base frame

    scanner();
    rule_stList();									//process function body

    return;
}

//rule:     <param-list> -> ( <param> <param-next>      |       ()
void rule_paramList(hashElem * activeElem)
{
    scanner();
    if (token.type != L_BRACKET)
        fatalError(ERR_SYNTAX);

	hTab * temp = hTabInit(INIT_ST_SIZE);				//create local symbol table

	tableStackPush(localSTstack, temp);					//push it to the table stack

    scanner();
    if (token.type == R_BRACKET)						//no parameters
       return;

    rule_param(activeElem);

    rule_paramNext(activeElem);

    return;
}

//rule:     <param> -> type id
void rule_param(hashElem * activeElem)    
{
	int type;

	switch (token.type)									// get the parameter type
	{
	case K_INT:
		type = VAR_INT;
		break;

	case K_DOUBLE:
		type = VAR_DOUBLE;
		break;

	case K_STRING:
		type = VAR_STRING;
		break;

	default:
		type = -1;
		fatalError(ERR_SYNTAX);
	}

    scanner();
    if (token.type != IDENTIFIER)
        fatalError(ERR_SYNTAX);

	addParam(activeElem, token.area, type);

	if (findVar(token.area))							// there already is a symbol with that name
		fatalError(ERR_AttemptedRedefVar);

	addVar(token.area, tableStackTop(localSTstack), type);	//add it to the symbol table

    return;
}

//rule:     <param-next> -> , <param> <param-next>      |   )
void rule_paramNext(hashElem * activeElem)
{
    scanner();
    
    if (token.type == COMMA)				//there should be another parameter
    {

        scanner();
        rule_param(activeElem);

        rule_paramNext(activeElem);

        return;
    }

    else  if (token.type == R_BRACKET)		//no more parameters
		return;

    fatalError(ERR_SYNTAX);
}   

//rule:     <st-list> -> <statement> ; <st-list>    |   }
void rule_stList()
{
	if (token.type == R_CURLY_BRACKET)
	{
		tableStackPop(localSTstack);
		generateInstruction(OP_DISPOSE_FRAME, NULL, NULL, NULL);
		return;
	}
    
    rule_statement(ALLOW_PUSH);

    scanner();
    rule_stList();

    return;
}

//rule:     <statement> -> type id <var-decl> ;   ||      id <assignment> ;		||      <keyword>   || { <st-list>
void rule_statement(int pushAllowed)
{
    if (token.type == IDENTIFIER)						//id -> assigment should follow
    {
		hashElem * assignee;
		if ((assignee = findVar(token.area)) == NULL)
			fatalError(ERR_UndefinedVariable);

        scanner();
		if (token.type == ASSIGNMENT)
		{
			rule_expression(assignee);
			return;
		}
    }

    else if (isKeyword(token.type))
    {
        rule_keyword();
		return;
    }

    else if (token.type == L_CURLY_BRACKET)				//new command block
    {
		if (pushAllowed)								//push new symboltable to the stack
		{
			hTab * temp = hTabInit(INIT_ST_SIZE);
			tableStackPush(localSTstack, temp);
			generateInstruction(OP_CREATE_FRAME, &tableStackTop(localSTstack)->numStoredElem, NULL, NULL);
		}

        scanner();
        rule_stList();
		return;
    }

    fatalError(ERR_SYNTAX);
}

//rule:     <var-decl> -> ;    |   = <expression> ;
void rule_varDecl(hashElem * assignee)
{
	int * varType = customMalloc(sizeof(int));
	*varType = assignee->data.type;

	generateInstruction(OP_CREATE_VAR, varType, NULL, varToFrame(assignee->key));		//create the variable in the frame

    scanner();

	if (token.type == SEMICOLON)					//only a declaration
		return;

	if (token.type == ASSIGNMENT)					//a definition
	{
		int retType = exprType(PrecedencniSA(tableStackTop(localSTstack), CALL_EXPRESSION));	//get the type of the expression

		if (!convertType(assignee->data.type, retType))											//check whether it can be converted
			fatalError(ERR_IncompatibleExpr);

		generateInstruction(OP_ASSIGN, instructionList->last->output, NULL, varToFrame(assignee->key));
	}
	else
		fatalError(ERR_SYNTAX);
}

//rule: <expression> -> <function-call>  || <built-in function> || TODO
void rule_expression(hashElem * assignee) 
{
	int retType;						//type of expression
	scanner();
	switch (token.type)
	{
		case IDENTIFIER:
			if (isFunct(token.area))
			{
				rule_funcCall(assignee);
				return;
			}
			else
			{
				ungetToken();
				retType = exprType(PrecedencniSA(tableStackTop(localSTstack), CALL_EXPRESSION));		//parse the expession

				if (!convertType(assignee->data.type, retType))				//check whether it can be converted
					fatalError(ERR_IncompatibleExpr);

				generateInstruction(OP_ASSIGN, instructionList->last->output, NULL, varToFrame(assignee->key));
				return;
			}
			break;

		case B_LENGTH:														//built-in functions
		case B_SUBSTR:
		case B_CONCAT:
		case B_FIND:
		case B_SORT:
			rule_builtIn(assignee);
			break;
		
		default:						//not a function call and doesn't start with an identifier, let the precedence syntactic analysis handle it
			ungetToken();
			retType = exprType(PrecedencniSA(tableStackTop(localSTstack), CALL_EXPRESSION));

			if (!convertType(assignee->data.type, retType))
					fatalError(ERR_IncompatibleExpr);

			generateInstruction(OP_ASSIGN, instructionList->last->output, NULL, varToFrame(assignee->key));
			return;
	}
}

//rule:     <auto-decl> -> id = <expression>
void rule_auto()
{
    scanner();
    if (token.type != IDENTIFIER)
        fatalError(ERR_SYNTAX);

	hashElem * assignee;

	if ((assignee = isDeclaredOnTheSameLevel(token.area)) != NULL)
		fatalError(ERR_AttemptedRedefVar);

	char * id = strDuplicate(token.area);							//save the variable name

    scanner();
    if (token.type != ASSIGNMENT)									//modifier auto requires assignment
        fatalError(ERR_SYNTAX);

	int  * retType = customMalloc(sizeof(int));
	*retType = exprType(PrecedencniSA(tableStackTop(localSTstack), CALL_EXPRESSION));

	addVar(id, tableStackTop(localSTstack), *retType);				//save the variable in the symbol table
	generateInstruction(OP_CREATE_VAR, retType, NULL, varToFrame(id));		//create the variable in the frame

	generateInstruction(OP_ASSIGN, instructionList->last->output, NULL, varToFrame(id));
} 

//rule:     <cin> -> >> id
int rule_cin()
{
    if (token.type != C_IN)
        fatalError(ERR_SYNTAX);

    scanner();
    if (token.type != IDENTIFIER)					//only variables can be "cinned"
        fatalError(ERR_SYNTAX);

	if (!findVar(token.area))
		fatalError(ERR_UndefinedVariable);

	generateInstruction(OP_CIN, NULL, NULL, varToFrame(token.area));

    return ERR_None;
}

// <cin-list> -> <cin> <cin-list>   ||  ;
void rule_cinList()
{
    scanner();

    if (token.type == SEMICOLON)
        return;
    else
    {
        rule_cin();
		rule_cinList();
    }
}

//rule:     <cout> -> << id
int rule_cout()
{
	if (token.type != C_OUT)
		fatalError(ERR_SYNTAX);

	void * data;

	tVarCoordinates * coordinates;

	scanner();
	switch (token.type)
	{
		case INT_NUMBER:
			data = &token.int_numb;
			break;

		case DOUBLE_NUMBER:
			data = &token.double_numb;
			break;

		case STRING:
			data = token.area;
			break;

		case IDENTIFIER:
			if (!findVar(token.area))					//looks up the variable
				fatalError(ERR_UndefinedVariable);

			coordinates = varToFrame(token.area);
			break;

		default:
			fatalError(ERR_SYNTAX);
	}

	if (token.type != IDENTIFIER)
		coordinates = constToVar(token.type, data);

	generateInstruction(OP_COUT, NULL, NULL, coordinates);

    return ERR_None;
}

// <cout-list> -> <cout> <cout-list>   ||  ;
void rule_coutList()
{
    scanner();

    if (token.type == SEMICOLON)
        return;
    else
    {
        rule_cout();
		rule_coutList();
    }
}

//rule:     <if-decl> -> ( <expression> ) <st-list> else <st-list>
int rule_if()
{
    PrecedencniSA(tableStackTop(localSTstack), CALL_CONDITION);		//process the condition

	tInstruction * elseJump = generateInstruction(OP_IFNOT_JUMP, instructionList->last->output, NULL, NULL);

    scanner();
	rule_statement(ALLOW_PUSH);										//if statement
	
	//we were in if statement, skip else statement
	tInstruction * skipElse = generateInstruction(OP_GOTO, NULL, NULL, NULL);

	generateInstruction(OP_NOP, NULL, NULL, NULL);					//jump past if statement
	elseJump->output = instructionList->last;

    scanner();
    if (token.type != K_ELSE)
    {
		generateInstruction(OP_NOP, NULL, NULL, NULL);					//jump past else statement
		skipElse->input1 = instructionList->last;
        ungetToken();
        return ERR_None;
    }
    
    scanner();
	rule_statement(ALLOW_PUSH);										//else statement

	generateInstruction(OP_NOP, NULL, NULL, NULL);					//jump past else statement
	skipElse->input1 = instructionList->last;

    return ERR_None;
}

//rule:     <return> -> expression
void rule_return()
{
    int retType = exprType(PrecedencniSA(tableStackTop(localSTstack), CALL_EXPRESSION));		//process the expression

	if (!convertType(retType, functionType))													//check whether if fits the function return type
		fatalError(ERR_IncompatibleExpr);

	int * type = customMalloc(sizeof(int));
	*type = functionType;

	generateInstruction(OP_RETURN, instructionList->last->output, type, NULL);
}

//rule:     <for-decl> -> type ( id <var-decl> <expression> id = <expression> <statement>
int rule_for()
{
	scanner();
	if (token.type != L_BRACKET)
		fatalError(ERR_SYNTAX);

	hTab * temp = hTabInit(INIT_ST_SIZE);				//create local symbol table
	tableStackPush(localSTstack, temp);
	generateInstruction(OP_CREATE_FRAME, &tableStackTop(localSTstack)->numStoredElem, NULL, NULL);

	scanner();
	int type;


	// INITIAL DECLARATON/DEFINITION
	switch (token.type)									//variable declaration/definition should follow
	{
	case K_INT:
	case K_DOUBLE:
	case K_STRING:
		type = getType(token.type);
		scanner();
		if (token.type != IDENTIFIER)
			fatalError(ERR_SYNTAX);

		hashElem * variable = addVar(token.area, tableStackTop(localSTstack), type);
		rule_varDecl(variable);
		break;

	case K_AUTO:
		rule_auto();
		break;
	default:
		fatalError(ERR_SYNTAX);
	}

	// CONDITION LABEL
	generateInstruction(OP_NOP, NULL, NULL, NULL);
	tInstruction * _for_condition = instructionList->last;

	//CONDITION
	PrecedencniSA(tableStackTop(localSTstack), CALL_EXPRESSION);			//condition for iteration

	//JUMP TO FOR LOOP BODY
	tInstruction * _for_body = generateInstruction(OP_IF_JUMP, instructionList->last->output, NULL, NULL);

	//ELSE JUMP TO FOR LOOP END
	tInstruction * _for_end = generateInstruction(OP_GOTO, NULL, NULL, NULL);

	//ASSIGNMENT LABEL
	generateInstruction(OP_NOP, NULL, NULL, NULL);
	tInstruction * _assignment = instructionList->last;

	//ASSIGNMENT
	scanner();
	if (token.type != IDENTIFIER)											//assignment should follow
		fatalError(ERR_SYNTAX);

	hashElem * controlVar;
	if ((controlVar = findVar(token.area)) == NULL)							//the variable hasn't been declared
		fatalError(ERR_UndefinedVariable);

	scanner();
	if (token.type != ASSIGNMENT)
		fatalError(ERR_SYNTAX);

	int exType = exprType(PrecedencniSA(tableStackTop(localSTstack), CALL_FOR));		//process the expression
	if (!convertType(controlVar->data.type, exType))							//check whether it can be converted
		fatalError(ERR_IncompatibleExpr);

	generateInstruction(OP_ASSIGN, instructionList->last->output, NULL, varToFrame(controlVar->key));

	//JUMP TO CONDITION
	generateInstruction(OP_GOTO, _for_condition, NULL, NULL);

	/*scanner();
	if (token.type != R_BRACKET)
		return ERR_SYNTAX;*/

		//FOR LOOP BODY LABEL
	generateInstruction(OP_NOP, NULL, NULL, NULL);
	_for_body->output = instructionList->last;

	scanner();
	rule_statement(DENY_PUSH);													//body of the loop

	//JUM TO THE ASSIGNMENT
	generateInstruction(OP_GOTO, _assignment, NULL, NULL);

	//FOR LOOP END LABEL
	generateInstruction(OP_NOP, NULL, NULL, NULL);
	_for_end->input1 = instructionList->last;

	if (token.type != R_CURLY_BRACKET)
	{
		tableStackPop(localSTstack);
		generateInstruction(OP_DISPOSE_FRAME, NULL, NULL, NULL);
	}
	return ERR_None;
}


//rule:     <keyword> -> auto <auto-decl>    ||      cin <cin> <cin-list>    ||  cout <cout> <cout-list>     ||   for <for-decl>  ||  if <if-decl>    ||  return <expression>

void rule_keyword()					//statement starts with a keyword, not an identifier
{
    switch (token.type)
    {
        case K_INT:
        case K_DOUBLE:
        case K_STRING:				//variable declaration/definition
        {
			int type = getType(token.type);
			scanner();
			
			if (token.type != IDENTIFIER)
				fatalError(ERR_SYNTAX);
			
			if (isDeclaredOnTheSameLevel(token.area) != NULL)
				fatalError(ERR_AttemptedRedefVar);						//the variable has already been declared in the same command block

			if (findElem(globalST, token.area) != NULL)					//there already is a function with given name
				fatalError(ERR_AttemptedRedefVar);

			hashElem * temp = addVar(token.area, tableStackTop(localSTstack), type);
			rule_varDecl(temp);
			return;
        }

        case K_CIN:
        {
            scanner();
			rule_cin();
			rule_cinList();
			return;
        }

        case K_COUT:
        {
            scanner();
    
			rule_cout();
            rule_coutList();
			return;
        }
    
        case K_IF:
            rule_if();
			return;
    
        case K_FOR:
            rule_for();
			return;
    
        case K_WHILE:
            rule_while();
			return;
    
        case K_DO:
            rule_do();
			return;
    
        case K_RETURN:
            rule_return();
			return;
    
        case K_AUTO:
            rule_auto();
			return;
    
        default:						//no compatible keyword was found
            fatalError(ERR_SYNTAX);
    }
}

//rule:      <while-loop> -> ( <expression> ) <statement>
int rule_while()
{
	hTab * temp = hTabInit(INIT_ST_SIZE);				//create local symbol table
	tableStackPush(localSTstack, temp);
	generateInstruction(OP_CREATE_FRAME, &tableStackTop(localSTstack)->numStoredElem, NULL, NULL);

	//CONDITION LABEL
	generateInstruction(OP_NOP, NULL, NULL, NULL);
	tInstruction * _condition = instructionList->last;

	PrecedencniSA(tableStackTop(localSTstack), CALL_CONDITION);		//process the loop condition

	//JUMP TO WHILE LOOP END
	tInstruction * _while_end = generateInstruction(OP_IFNOT_JUMP, instructionList->last->output, NULL, NULL);

	scanner();
	rule_statement(DENY_PUSH);										//loop body

	//JUMP TO CONDITION
	generateInstruction(OP_GOTO, _condition, NULL, NULL);

	//WHILE LOOP END LABEL
	generateInstruction(OP_NOP, NULL, NULL, NULL);					//jump past if statement
	_while_end->output = instructionList->last;

	if (token.type != R_CURLY_BRACKET)
	{
		tableStackPop(localSTstack);
		generateInstruction(OP_DISPOSE_FRAME, NULL, NULL, NULL);
	}

    return ERR_None;
}

//rule:     <do-loop> -> <statement> while ( <expression> );
int rule_do()
{    
	hTab * temp = hTabInit(INIT_ST_SIZE);				//create local symbol table
	tableStackPush(localSTstack, temp);
	generateInstruction(OP_CREATE_FRAME, &tableStackTop(localSTstack)->numStoredElem, NULL, NULL);

	//DO LOOP LABEL
	generateInstruction(OP_NOP, NULL, NULL, NULL);
	tInstruction * _do_loop = instructionList->last;

    scanner();
	rule_statement(DENY_PUSH);

    scanner();
    if (token.type != K_WHILE)
        fatalError(ERR_SYNTAX);

    /*scanner();
    if (token.type != L_BRACKET)
        fatalError(ERR_SYNTAX);*/

	PrecedencniSA(tableStackTop(localSTstack), CALL_CONDITION);

	generateInstruction(OP_IF_JUMP, instructionList->last->output, NULL, _do_loop);

    /*scanner();
    if (token.type != R_BRACKET)
        return ERR_SYNTAX;*/ //?????

	tableStackPop(localSTstack);
	generateInstruction(OP_DISPOSE_FRAME, NULL, NULL, NULL);

    scanner();
    if (token.type != SEMICOLON)
        fatalError(ERR_SYNTAX);

    return ERR_None;
}

int getType(int tokenType)					//converts token type to tSymbol variable type
{
    switch (tokenType)
    {
        case K_INT:
            return VAR_INT;

        case K_DOUBLE:
            return VAR_DOUBLE;

        case K_STRING:
            return VAR_STRING;

        default:
            return -1;
    }
}

int exprType(int type)					//converts lex_an variable type to tSymbolType variable type
{
	switch (type)
	{
	case INT_NUMBER:
		return VAR_INT;

	case DOUBLE_NUMBER:
		return VAR_DOUBLE;

	case STRING:
		return VAR_STRING;

	default:
		return -1;
	}
}

int getFuncType(int tokenType)					//converts token type to tSymbol function type
{
    switch (tokenType)
    {
        case K_INT:
            return FUNC_INT;

        case K_DOUBLE:
            return FUNC_DOUBLE;

        case K_STRING:
            return FUNC_STRING;

        default:
            return -1;
    }
}

int compareSymbol(hashElem * elem, hashElem * activeElem)                          //compares function symbols
{
    if (elem->data.type != activeElem->data.type)
        return 0;

	return compareParams(elem, activeElem);
}

int compareParams(hashElem * elem1, hashElem * elem2)								//compares function parameters
{
	if (elem1->data.numberOfParams != elem2->data.numberOfParams)
		return 0;

	for (unsigned int i = 0; i < elem1->data.numberOfParams; i++)
	{
		if (elem1->data.params[i].type != elem2->data.params[i].type)
			return 0;
		if (strcmp(elem1->data.params[i].key, elem2->data.params[i].key) != 0)
			return 0;
	}

	return 1;
}

int compareParamTypes(hashElem * elem1, hashElem * elem2)						//compares function parameter types at function call
{
	if (elem1->data.numberOfParams != elem2->data.numberOfParams)
		return 0;

	for (unsigned int i = 0; i < elem1->data.numberOfParams; i++)
		if (!convertType(elem1->data.params[i].type, elem2->data.params[i].type))		//checks whether the parameter can be converted
			return 0;

	return 1;
}

hashElem * addVar(char * key, hTab * table, int type)								//adds variable to the symbol table
{
	tData temp;
	temp.type = type;
	temp.state = DECLARED;
	temp.numberOfParams = 0;
	temp.params = NULL;
	temp.index = table->numStoredElem;

	return addElem(table, key, &temp);
}

hashElem * findVar(char *key)									//finds the variable in the symbol table stack
{
	hashElem * temp;


	for (int i = localSTstack->top; i >= 0; i--)
		if ((temp = findElem(getTableStackElem(localSTstack, i), key)) != NULL)
			return temp;

	return NULL;										// the symbol wasn't found
}

hashElem * isDeclaredOnTheSameLevel(char *key)						//finds the variable declared in the current command block
{
	return findElem(tableStackTop(localSTstack), key);
}

tParam * addParam(hashElem * elem, char * key, tSymbolType type)			//adds function parameters
{
	tParam * temp = customRealloc(elem->data.params, (elem->data.numberOfParams + 1) * sizeof(tParam));

	elem->data.params = temp;
	elem->data.params[elem->data.numberOfParams].type = type;
	elem->data.params[elem->data.numberOfParams].key = strDuplicate(key);

	++elem->data.numberOfParams;
	return temp;
}

hashElem * isFunct(char * key)
{
	return findElem(globalST, key);
}

int rule_funcCall(hashElem * assignee)	//id ( <call_list> ;
{
	hashElem funcCall;
	funcCall.key = NULL;
	funcCall.data.params = NULL;
	hashElemInit(&funcCall);

	funcCall.key = strDuplicate(token.area);
	if (!convertType(assignee->data.type, findElem(globalST, funcCall.key)->data.type))
		fatalError(ERR_IncompatibleExpr);

	generateInstruction(OP_CREATE_FRAME, findElem(globalST, funcCall.key)->data.baseFrameSize, NULL, NULL);
	int numberOfParams = findElem(globalST, funcCall.key)->data.numberOfParams;
	
	// create frame variables for parameters
	for (int i = 0; i < numberOfParams; i++)
	{
		tVarCoordinates * param = customMalloc(sizeof(tVarCoordinates));
		param->frameOffset = 0;
		param->index = i;
		int * type = customMalloc(sizeof(int));
		*type = (int) findElem(globalST, funcCall.key)->data.params[i].type;
		generateInstruction(OP_CREATE_VAR, type, NULL, param);
	}

	int paramIndex = 0;

	scanner();
	if (token.type != L_BRACKET)
		fatalError(ERR_SYNTAX);
    
	rule_callParam(&funcCall, &paramIndex, -1);

	if (!compareParamTypes(&funcCall, findElem(globalST, funcCall.key)))
		fatalError(ERR_ParamType);
	
	scanner();
	if (token.type != SEMICOLON)
		fatalError(ERR_SYNTAX);

	generateInstruction(OP_FUNC_CALL, findElem(globalST, funcCall.key)->data.instructions, NULL, NULL);

	//store the return value in the variable
	generateInstruction(OP_GET_RETURN_VALUE, NULL, NULL, varToFrame(assignee->key));

	hashElemInit(&funcCall);
	return ERR_None;
}

void rule_callParam(hashElem * funcCall, int  * paramIndex, int builtIn)
{
	scanner();

	processParam(paramIndex, funcCall, builtIn);

	rule_callParamList(funcCall, paramIndex, builtIn);
	return;
}

void rule_callParamList(hashElem * funcCall, int  * paramIndex, int builtIn)
{
	scanner();
	switch (token.type)
	{
		case COMMA:
			break;

		case R_BRACKET:
			return;

		default:
			fatalError(ERR_SYNTAX);
	}

	scanner();
	
	processParam(paramIndex, funcCall, builtIn);
}

int rule_builtIn(hashElem * assignee)
{
	int funcType;
	int * function = customMalloc(sizeof(int));
	*function = token.type;
	int * paramCount = customMalloc(sizeof(int));
	
	switch (token.type)					//get the function return type
	{
		case B_LENGTH:
		case B_FIND:
			funcType = FUNC_INT;
			break;

		case B_SUBSTR:
		case B_CONCAT:
		case B_SORT:
			funcType = FUNC_STRING;
			break;
	}

	//CREATE "BASE" FRAMES
	switch (token.type)
	{
		case B_LENGTH:
		case B_SORT:
			*paramCount = 1;
			generateInstruction(OP_CREATE_FRAME, paramCount, NULL, NULL);
			break;

		case B_CONCAT:
		case B_FIND:
			*paramCount = 2;
			generateInstruction(OP_CREATE_FRAME, paramCount, NULL, NULL);
			break;

		case B_SUBSTR:
			*paramCount = 3;
			generateInstruction(OP_CREATE_FRAME, paramCount, NULL, NULL);
			break;
	}

	// create frame variables for parameters
	for (int i = 0; i < *paramCount; i++)
	{
		tVarCoordinates * param = customMalloc(sizeof(tVarCoordinates));
		param->frameOffset = 0;
		param->index = i;
		int * type = customMalloc(sizeof(int));
		switch (*function)
		{
			case B_LENGTH:
			case B_SORT:
			case B_CONCAT:
			case B_FIND:
				*type = VAR_STRING;
				break;

			case B_SUBSTR:
				if (i == 0)
					*type = VAR_STRING;
				else
					*type = VAR_INT;
		}

		generateInstruction(OP_CREATE_VAR, type, NULL, param);
	}



	hashElem funcCall;
	funcCall.key = NULL;
	funcCall.data.params = NULL;
	hashElemInit(&funcCall);

	scanner();
	if (token.type != L_BRACKET)
		fatalError(ERR_SYNTAX);

	int paramIndex = 0;
	rule_callParam(&funcCall, &paramIndex, *function);					//scan for all the parameters

	switch (*function)							//compare parameters with required parameters
	{
		case B_LENGTH:
		case B_SORT:
			if (funcCall.data.numberOfParams != 1)
				fatalError(ERR_ParamNumber);

			if (!convertType(funcCall.data.params[0].type, VAR_STRING))
				fatalError(ERR_ParamType);
			break;

		case B_SUBSTR:
			if (funcCall.data.numberOfParams != 3)
				fatalError(ERR_ParamNumber);

			if (!convertType(funcCall.data.params[0].type, VAR_STRING))
				fatalError(ERR_ParamType);

			if (!convertType(funcCall.data.params[1].type, VAR_INT))
				fatalError(ERR_ParamType);

			if (!convertType(funcCall.data.params[2].type, VAR_INT))
				fatalError(ERR_ParamType);
			break;

		case B_CONCAT:
		case B_FIND:
			if (funcCall.data.numberOfParams != 2)
				fatalError(ERR_ParamNumber);

			if (!convertType(funcCall.data.params[0].type, VAR_STRING))
				fatalError(ERR_ParamType);

			if (!convertType(funcCall.data.params[1].type, VAR_STRING))
				fatalError(ERR_ParamType);
			break;
	}

	if (!convertType(assignee->data.type, funcType))			//check whether the return value can be converted
		fatalError(ERR_IncompatibleExpr);

	scanner();
	if (token.type != SEMICOLON)
		fatalError(ERR_SYNTAX);

	generateInstruction(OP_BUILT_IN, function, NULL, varToFrame(assignee->key));
	hashElemInit(&funcCall);
	return ERR_None;
}

int convertType(tSymbolType inType, tSymbolType outType)			//converts inType to outType
{
	switch (inType)
	{
		case VAR_INT:
			switch (outType)
			{
				case VAR_INT:
				case FUNC_INT:
					return 1;			//no conversion needed

				case VAR_DOUBLE:
				case FUNC_DOUBLE:
					return 1;

				default: 
					return 0;
			}
			break;

		case VAR_DOUBLE:
			switch (outType)
			{
				case VAR_DOUBLE:
				case FUNC_DOUBLE:
					return 1;			//no conversion needed

				case VAR_INT:
				case FUNC_INT:
					return 1;

				default:
					return 0;
			}
			break;

		case VAR_STRING:
			switch (outType)
			{
				case VAR_STRING:
				case FUNC_STRING:
					return 1;			//no conversion needed

				default:
					return 0;			//string can't be converted to other types
			}

		default:
			return 0;

	}
}

void processParam(int * paramIndex, hashElem * funcCall, int builtIn)
{
	hashElem * temp;
	tVarCoordinates * variableOut = customMalloc(sizeof(tVarCoordinates));

	variableOut->frameOffset = 0;
	variableOut->index = *paramIndex;
	//variableOut->operand = coordinates;
	
	++(*paramIndex);

	void * data;

	tVarCoordinates * coordinates;

	switch (token.type)
	{
		case R_BRACKET:
			return;

		case INT_NUMBER:
			addParam(funcCall, "", VAR_INT);
			data = &token.int_numb;
			break;

		case DOUBLE_NUMBER:
			addParam(funcCall, "", VAR_DOUBLE);
			data = &token.double_numb;
			break;

		case STRING:
			addParam(funcCall, "", VAR_STRING);
			data = token.area;
			break;

		case IDENTIFIER:
			if ((temp = findVar(token.area)) == NULL)
				fatalError(ERR_UndefinedVariable);

			addParam(funcCall, "", temp->data.type);

			coordinates = varToFrame(token.area);
			break;

		default:
			fatalError(ERR_SYNTAX);
	}

	if (token.type != IDENTIFIER)
		coordinates = constToVar(token.type, data);

	generateInstruction(OP_ASSIGN, coordinates, NULL, variableOut);
}

void checkFuncDefinitions()
{
	hashElem * temp;
	for (unsigned int i = 0; i < globalST->size; i++)
	{
		temp = globalST->table[i];
		
		while (temp != NULL)
		{
			if (temp->data.state != DEFINED)
				fatalError(ERR_UndefinedFunction);

			temp = temp->next;
		}
	}
}

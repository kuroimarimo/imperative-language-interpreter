#include "parser.h"

int isKeyword(int tokenType)
{
    return (tokenType >= K_CIN) && (tokenType <= K_DO);
}

void parse()
{
    hashElem activeElem;			//informations about curently parsed function
    activeElem.key = NULL;

    globalST = hTabInit(INIT_ST_SIZE);
	localSTstack = tableStackInit(INIT_ST_SIZE);

    rule_funcdef(&activeElem);		//process function definition/declaration
    
	tableStackDispose(localSTstack);

	if (!isFunct(F_MAIN) || (findElem(globalST, F_MAIN)->data.state != DEFINED))				//there's no main
		fatalError(ERR_UndefinedFunction);
}

// rule:    <prog> -> type id <param-list> <func-defined>   || EOF
void rule_funcdef(hashElem * activeElem)
{
    hashElemInit(activeElem);

    scanner();
    if (token.type == EOF)				//the whole file has been processed
        return;

    /*if ((token.type != K_INT) && (token.type != K_DOUBLE) && (token.type != K_STRING))
        return ERR_SYNTAX;*/

	switch (token.type)
	{
		case K_INT:
		case K_DOUBLE:
		case K_STRING:
			break;

		default:
			fatalError(ERR_SYNTAX);
	}


    activeElem->data.type = getFuncType(token.type);				//get function type
    activeElem->data.state = DECLARED;

    scanner();
    if (token.type != IDENTIFIER)
       fatalError(ERR_SYNTAX);

	activeElem->key = strDuplicate(token.area);						//save the function name
    
    rule_paramList(activeElem);							//process parameters

	//main should be of type int and with no parameters
    if (!strcmp(activeElem->key, F_MAIN) && ((activeElem->data.numberOfParams != 0) || (activeElem->data.type != FUNC_INT)))
        fatalError(ERR_UndefinedFunction);

    rule_funcDefined(activeElem);		//process the rest of definition/declaration

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
		case SEMICOLON:						//only a declaration, nothing more to be done here
			tableStackPop(localSTstack);
			return;

		case L_CURLY_BRACKET:				//function definition
			break;

		default: 
			fatalError(ERR_SYNTAX);
	}

    if (temp->data.state == DEFINED)		
        fatalError(ERR_AttemptedRedefFunction);	//function is already defined

	temp->data.state = DEFINED;

    scanner();
    rule_stList();				//process function body

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

	switch (token.type)					// get the parameter type
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
		fatalError(ERR_SYNTAX);
	}

    scanner();
    if (token.type != IDENTIFIER)
        fatalError(ERR_SYNTAX);

	addParam(activeElem, token.area, type);

	if (findVar(token.area))							// there already is a symbol with that name
		fatalError(ERR_AttemptedRedefVar);

    //pridanie ID premennej do tabulky

	addVar(token.area, tableStackTop(localSTstack), type);

    return;
}

//rule:     <param-next> -> , <param> <param-next>      |   )
void rule_paramNext(hashElem * activeElem)
{
    scanner();
    
    if (token.type == COMMA)
    {

        scanner();
        rule_param(activeElem);

        rule_paramNext(activeElem);

        return;
    }

    else  if (token.type == R_BRACKET)
		return;

    fatalError(ERR_SYNTAX);
}   

//rule:     <st-list> -> <statement> ; <st-list>    |   }
void rule_stList()
{
	if (token.type == R_CURLY_BRACKET)
	{
		tableStackPop(localSTstack);
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
    if (token.type == IDENTIFIER)
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

    else if (token.type == L_CURLY_BRACKET)
    {
		hTab * temp = hTabInit(INIT_ST_SIZE);

		if (pushAllowed)
			tableStackPush(localSTstack, temp);

        scanner();
        rule_stList();
		return;
    }

    return;
}

//rule:     <var-decl> -> ;    |   = <expression> ;
void rule_varDecl(hashElem * assignee)
{
    scanner();

    if (token.type == SEMICOLON)
        return;

    if (token.type == ASSIGNMENT)
        PrecedencniSA(tableStackTop(localSTstack), CALL_EXPRESSION);
	else
		fatalError(ERR_SYNTAX);
}

void rule_expression(hashElem * assignee)
{
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
				PrecedencniSA(tableStackTop(localSTstack), CALL_EXPRESSION);		//TODO pouzivat cely stack, treba na to vytvorit funkciu
				return;
			}
			break;

		case B_LENGTH:
		case B_SUBSTR:
		case B_CONCAT:
		case B_FIND:
		case B_SORT:
			rule_builtIn(assignee);
			break;
		default:
			ungetToken();
			PrecedencniSA(tableStackTop(localSTstack), CALL_EXPRESSION);		//TODO pouzivat cely stack, treba na to vytvorit funkciu
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

    scanner();
    if (token.type != ASSIGNMENT)
        fatalError(ERR_SYNTAX);

    rule_expression(assignee);
} 

//rule:     <cin> -> >> id
int rule_cin()
{
    if (token.type != C_IN)
        fatalError(ERR_SYNTAX);

    scanner();
    if (token.type != IDENTIFIER)
        fatalError(ERR_SYNTAX);

	if (!findVar(token.area))
		fatalError(ERR_UndefinedVariable);

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

    scanner();
	switch (token.type)
	{
		case STRING:
		case INT_NUMBER:
		case DOUBLE_NUMBER:
			break;

		case IDENTIFIER:
			if (!findVar(token.area))
				fatalError(ERR_UndefinedVariable);
			break;

		default:
			fatalError(ERR_SYNTAX);
	}

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
    //scanner();
    /*if (token.type != L_BRACKET)
        fatalError(ERR_SYNTAX);*/

    PrecedencniSA(tableStackTop(localSTstack), CALL_CONDITION);

    /*scanner();
    if (token.type != R_BRACKET)
        fatalError(ERR_SYNTAX);*/

    scanner();
	rule_statement(ALLOW_PUSH);

    scanner();
    if (token.type != K_ELSE)
    {
        ungetToken();
        return ERR_None;
    }
    
    scanner();
	rule_statement(ALLOW_PUSH);

    return ERR_None;
}

//rule:     <return> -> expression
void rule_return()
{
    PrecedencniSA(tableStackTop(localSTstack), CALL_EXPRESSION);				//TODO
}

//rule:     <for-decl> -> ( id <var-decl> <expression> id = <expression> ) { <st-list>
int rule_for()
{
    scanner();
	if (token.type != L_BRACKET)
		fatalError(ERR_SYNTAX);

	hTab * temp = hTabInit(INIT_ST_SIZE);

	tableStackPush(localSTstack, temp);

    scanner();
	int type;

	switch (token.type)
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

	PrecedencniSA(tableStackTop(localSTstack), CALL_EXPRESSION);

    scanner();
    if (token.type != IDENTIFIER)
        fatalError(ERR_SYNTAX);

	hashElem * controlVar;
	if ((controlVar = findVar(token.area)) == NULL)
		fatalError(ERR_UndefinedVariable);

    scanner();
    if (token.type != ASSIGNMENT)
        fatalError(ERR_SYNTAX);

	PrecedencniSA(tableStackTop(localSTstack), CALL_CONDITION);		//rovno prsa namiesto expression?

    scanner();
    /*if (token.type != R_BRACKET)
        return ERR_SYNTAX;*/
	
    scanner();
    rule_statement(DENY_PUSH);

	tableStackPop(localSTstack);
	return ERR_None;
}


//rule:     <keyword> -> auto <auto-decl>    ||      cin <cin> <cin-list>    ||  cout <cout> <cout-list>     ||   for <for-decl>  ||  if <if-decl>    ||  return <expression>

void rule_keyword()
{
    switch (token.type)
    {
        case K_INT:
        case K_DOUBLE:
        case K_STRING:
        {
			int type = getType(token.type);
			scanner();
			
			if (token.type != IDENTIFIER)
				fatalError(ERR_SYNTAX);
			
			if (isDeclaredOnTheSameLevel(token.area) != NULL)
				fatalError(ERR_AttemptedRedefVar);

			if (findElem(globalST, token.area) != NULL)
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
    
        default:
            fatalError(ERR_SYNTAX);
    }
}

//rule:      <while-loop> -> ( <expression> ) <statement>
int rule_while()
{
    scanner();
    if (token.type != L_BRACKET)
        fatalError(ERR_SYNTAX);

	PrecedencniSA(tableStackTop(localSTstack), CALL_CONDITION);

    scanner();
    if (token.type != R_BRACKET)
        fatalError(ERR_SYNTAX);

    scanner();
	rule_statement(ALLOW_PUSH);

    //generovanie skoku and stuff

    return ERR_None;
}

//rule:     <do-loop> -> <statement> while ( <expression> );
int rule_do()
{    
    scanner();
	rule_statement(ALLOW_PUSH);

    scanner();
    if (token.type != K_WHILE)
        fatalError(ERR_SYNTAX);

    scanner();
    if (token.type != L_BRACKET)
        fatalError(ERR_SYNTAX);

	PrecedencniSA(tableStackTop(localSTstack), CALL_CONDITION);

    /*scanner();
    if (token.type != R_BRACKET)
        return ERR_SYNTAX;*/ //?????

    scanner();
    if (token.type != SEMICOLON)
        fatalError(ERR_SYNTAX);

    return ERR_None;
}

int getType(int tokenType)
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

int getFuncType(int tokenType)
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

int compareSymbol(hashElem * elem, hashElem * activeElem)                          //compares elem with global variable activeElem
{
    if (elem->data.type != activeElem->data.type)
        return 0;

	return compareParams(elem, activeElem);
}

int compareParams(hashElem * elem1, hashElem * elem2)
{
	if (elem1->data.numberOfParams != elem2->data.numberOfParams)
		return 0;

	for (int i = 0; i < elem1->data.numberOfParams; i++)
	{
		if (elem1->data.params[i].type != elem2->data.params[i].type)
			return 0;
		if (strcmp(elem1->data.params[i].key, elem2->data.params[i].key) != 0)
			return 0;
	}

	return 1;
}

int compareParamTypes(hashElem * elem1, hashElem * elem2)
{
	if (elem1->data.numberOfParams != elem2->data.numberOfParams)
		return 0;

	for (int i = 0; i < elem1->data.numberOfParams; i++)
		if (!convertType(elem1->data.params[i].type, elem2->data.params[i].type))
			return 0;

	return 1;
}

hashElem * addVar(char * key, hTab * table, int type)
{
	tData temp;
	temp.type = type;
	temp.state = DECLARED;
	temp.numberOfParams = 0;
	temp.params = NULL;
	//temp.fParamTypes = NULL;
	//temp.localTable = NULL;

	return addElem(table, key, &temp);
}

hashElem * findVar(char *key)									//TODO lepsie meno
{
	hashElem * temp;

	for (int i = localSTstack->top; i >= 0; i--)
		if ((temp = findElem(getTableStackElem(localSTstack, i), key)) != NULL)
			return temp;
	
	return NULL;										// the symbol wasn't found
}

hashElem * isDeclaredOnTheSameLevel(char *key)						//TODO lepsie meno >_<
{
	return findElem(tableStackTop(localSTstack), key);
}

tParam * addParam(hashElem * elem, char * key, tSymbolType type)
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

int rule_funcCall(hashElem * assignee)	//id (<call_list>;
{
	hashElem funcCall;
	funcCall.key = NULL;
	funcCall.data.params = NULL;
	hashElemInit(&funcCall);

	funcCall.key = strDuplicate(token.area);
	if (!convertType(assignee->data.type, findElem(globalST, funcCall.key)->data.type))
		fatalError(ERR_IncompatibleExpr);

	scanner();
	if (token.type != L_BRACKET)
		fatalError(ERR_SYNTAX);
    
	rule_callParam(&funcCall);

	if (!compareParamTypes(&funcCall, findElem(globalST, funcCall.key)))
		fatalError(ERR_ParamType);
	
	scanner();
	if (token.type != SEMICOLON)
		fatalError(ERR_SYNTAX);

	hashElemInit(&funcCall);
	return ERR_None;
}

void rule_callParam(hashElem * funcCall)
{
	scanner();

	hashElem * temp;	
	switch (token.type)
	{
		case R_BRACKET:
			return;

		case IDENTIFIER:
			if ((temp = findVar(token.area)) == NULL)
				fatalError(ERR_UndefinedVariable);

			addParam(funcCall, "", temp->data.type);
			rule_callParamList(funcCall);
			return;

		case INT_NUMBER:
			addParam(funcCall, "", VAR_INT);
			rule_callParamList(funcCall);
			return;

		case DOUBLE_NUMBER:
			addParam(funcCall, "", VAR_DOUBLE);
			rule_callParamList(funcCall);
			return;

		case STRING:
			addParam(funcCall, "", VAR_STRING);
			rule_callParamList(funcCall);
			return;

		default:
			fatalError(ERR_SYNTAX);
	}
}

void rule_callParamList(hashElem * funcCall)
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

	hashElem * temp;
	scanner();
	switch (token.type)
	{
		case IDENTIFIER:
			if ((temp = findVar(token.area)) == NULL)
				fatalError(ERR_UndefinedVariable);

			addParam(funcCall, "", temp->data.type);
			rule_callParamList(funcCall);
			return;

		case INT_NUMBER:
			addParam(funcCall, "", VAR_INT);
			rule_callParamList(funcCall);
			return;

		case DOUBLE_NUMBER:
			addParam(funcCall, "", VAR_DOUBLE);
			rule_callParamList(funcCall);
			return;

		case STRING:
			addParam(funcCall, "", VAR_STRING);
			rule_callParamList(funcCall);
			return;

		default:
			fatalError(ERR_SYNTAX);
	}	
}

void rule_BLength()
{
	scanner();
	if (token.type != L_BRACKET)
		fatalError(ERR_SYNTAX);

	scanner();

	switch (token.type)
	{
		case STRING:
			break;
		
		case IDENTIFIER:
			if (findVar(token.area)->data.type == VAR_STRING)
				break;
			else
				fatalError(ERR_ParamType);
		
		case R_BRACKET:
			fatalError(ERR_ParamNumber);
		
		default:
			fatalError(ERR_SYNTAX);
	}

	scanner();
	switch (token.type)
	{
		case R_BRACKET:
			break;

		case COMMA:
			scanner();
			switch (token.type)
			{
				case IDENTIFIER:
				case INT_NUMBER:
				case DOUBLE_NUMBER:
				case STRING:
					fatalError(ERR_ParamNumber);
				default:
					fatalError(ERR_SYNTAX);
			}

		default:
			fatalError(ERR_SYNTAX);
	}

	scanner();
	switch (token.type)
	{
		case SEMICOLON:
			return;

		default:
			fatalError(ERR_SYNTAX);
	}
}

int rule_builtIn(hashElem * assignee)
{
	int functionType;
	switch (token.type)
	{
		case B_LENGTH:
		case B_FIND:
			functionType = FUNC_INT;
			break;

		case B_SUBSTR:
		case B_CONCAT:
		case B_SORT:
			functionType = FUNC_STRING;
			break;
	}

	hashElem funcCall;
	funcCall.key = NULL;
	funcCall.data.params = NULL;
	hashElemInit(&funcCall);

	//funcCall.key = strDuplicate(token.area);

	scanner();
	if (token.type != L_BRACKET)
		fatalError(ERR_SYNTAX);

	rule_callParam(&funcCall);

	/*if (!compareParamTypes(&funcCall, findElem(globalST, funcCall.key)))
		fatalError(ERR_ParamType);*/

	switch (functionType)
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

	if (!convertType(assignee->data.type, functionType))
		fatalError(ERR_IncompatibleExpr);

	scanner();
	if (token.type != SEMICOLON)
		fatalError(ERR_SYNTAX);

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
					//generovanie inštrukcie na konverziu
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
					//generovanie inštrukcie na konverziu
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
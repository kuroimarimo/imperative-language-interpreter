#include "parser.h"

int isKeyword(int tokenType)
{
    return (tokenType >= K_AUTO) && (tokenType <= K_DO);
}

int parse()
{
    hashElem activeElem;			//informations about curently parsed function
    activeElem.key = NULL;

    globalST = hTabInit(INIT_ST_SIZE);
	if ((localSTstack = tableStackInit(INIT_ST_SIZE)) == NULL)
		return ERR_AllocFailed;


    int returnValue = rule_funcdef(&activeElem);		//process function definition/declaration
    

	//cleanup
    if (activeElem.key)
    {
        free(activeElem.data.params);
        free(activeElem.key);
    }
	tableStackDispose(localSTstack);

	if (returnValue != ERR_None)
		return returnValue;

	if (!isFunct(F_MAIN))					//there's no main
		return ERR_UndefinedFunction;

	//call interpreter

    return returnValue;
}

// rule:    <prog> -> type id <param-list> <func-defined>   || EOF
int rule_funcdef(hashElem * activeElem)
{
    hashElemInit(activeElem);

    scanner();
    if (token.type == EOF)				//the whole file has been processed
        return ERR_None;

    if ((token.type != K_INT) && (token.type != K_DOUBLE) && (token.type != K_STRING))
        return ERR_SYNTAX;


    activeElem->data.type = getFuncType(token.type);				//get function type
    activeElem->data.state = DECLARED;

    scanner();
    if (token.type != IDENTIFIER)
        return ERR_SYNTAX;

    if ((activeElem->key = malloc((strlen(token.area) + 1) * sizeof(char))) == NULL)
        return ERR_AllocFailed;
    strcpy(activeElem->key, token.area);                            //save the function name
    
    int error = rule_paramList(activeElem);							//process parameters
    if (error != ERR_None)
        return error;

	//main should be of type int and with no parameters
    if (!strcmp(activeElem->key, F_MAIN) && ((activeElem->data.numberOfParams != 0) || (activeElem->data.type != FUNC_INT)))
    {
        return ERR_UndefinedFunction;
    }

    error = rule_funcDefined(activeElem);		//process the rest of definition/declaration
    if (error != ERR_None)
        return error;

    return rule_funcdef(activeElem);
}

//rule:     <func-defined> -> { <st-list>        ||      ; 
int rule_funcDefined(hashElem * activeElem)
{
    hashElem * temp = findElem(globalST, activeElem->key);

    if (temp == NULL)																
    {		
			//there's no such function declared in symbol table, create it then
            if ((temp = addElem(globalST, activeElem->key, &activeElem->data)) == NULL)
                    return ERR_AllocFailed;
    }

    // the function has already been declared   
    else if (!compareSymbol(temp, activeElem))
        return ERR_AttemptedRedefFunction;

    scanner();
	switch (token.type)
	{
		case SEMICOLON:						//only a declaration, nothing more to be done here
			tableStackPop(localSTstack);
			return ERR_None;

		case L_CURLY_BRACKET:				//function definition
			break;

		default: 
			return ERR_SYNTAX;
	}

    if (temp->data.state == DEFINED)		
        return ERR_AttemptedRedefFunction;	//function is already defined

	temp->data.state = DEFINED;

    scanner();
    int error = rule_stList();				//process function body
    if (error != ERR_None)
        return error;

    return ERR_None;
}

//rule:     <param-list> -> ( <param> <param-next>      |       ()
int rule_paramList(hashElem * activeElem)
{
    scanner();
    if (token.type != L_BRACKET)
        return ERR_SYNTAX;

	hTab * temp;
	if ((temp = hTabInit(INIT_ST_SIZE)) == NULL)		//create local symbol table
		return ERR_AllocFailed;

	if (tableStackPush(localSTstack, temp) == NULL)		//push it to the table stack
	{
		hTabFree(temp);
		return ERR_AllocFailed;
	}

    scanner();
    if (token.type == R_BRACKET)						//no parameters
        return ERR_None;

    int error = rule_param(activeElem);
    if (error != ERR_None)
        return error;

    error = rule_paramNext(activeElem);
    if (error != ERR_None)
        return error;

    return ERR_None;

}

//rule:     <param> -> type id
int rule_param(hashElem * activeElem)    
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
		return ERR_SYNTAX;
	}

	//activeElem->data.fParamTypes = appendChar(activeElem->data.fParamTypes, paramTypeToChar(token.type));    //add param type
    scanner();
    if (token.type != IDENTIFIER)
        return ERR_SYNTAX;

	if (addParam(activeElem, token.area, type) == NULL)
		return ERR_AllocFailed;

	if (findVar(token.area))							// there already is a symbol with that name
		return ERR_AttemptedRedefVar;

    //pridanie ID premennej do tabulky

	if (addVar(token.area, tableStackTop(localSTstack), type) == NULL)
		return ERR_AllocFailed;

    return ERR_None;
}

//rule:     <param-next> -> , <param> <param-next>      |   )
int rule_paramNext(hashElem * activeElem)
{
    scanner();
    
    if (token.type == COMMA)
    {

        scanner();
        int error = rule_param(activeElem);
        if (error != ERR_None)
            return error;

        error = rule_paramNext(activeElem);
        if (error != ERR_None)
            return error;

        return ERR_None;
    }

    else 
    {
        if (token.type == R_BRACKET)
            return ERR_None;
    }

    return ERR_SYNTAX;
}   

//rule:     <st-list> -> <statement> ; <st-list>    |   }
int rule_stList()
{
    int error;
    

	if (token.type == R_CURLY_BRACKET)
	{
		tableStackPop(localSTstack);
		return ERR_None;
	}
    
    error = rule_statement();
        if (error != ERR_None)
            return error;

    scanner();
    error = rule_stList();
    if (error != ERR_None)
		return error;

    return ERR_None;
}

//rule:     <statement> -> type id <var-decl> ;   ||      id <assignment> ;		||      <keyword>   || { <st-list>
int rule_statement()
{
    if (token.type == IDENTIFIER)
    {
		hashElem * assignee;
		if ((assignee = findVar(token.area)) == NULL)
			return ERR_UndefinedVariable;

        scanner();
        if (token.type == ASSIGNMENT)
            return rule_expression(assignee);
    }

    else if (isKeyword(token.type))
    {
        return rule_keyword();
    }

    else if (token.type == L_CURLY_BRACKET)
    {
		hTab * temp = hTabInit(INIT_ST_SIZE);
		if (temp == NULL)
			return ERR_AllocFailed;

		if ((tableStackPush(localSTstack, temp)) == NULL)
		{
			free(temp);
			return ERR_AllocFailed;
		}

        scanner();
        return rule_stList();
    }

    return ERR_SYNTAX;
}

//rule:     <var-decl> -> ;    |   = <expression> ;
int rule_varDecl(hashElem * assignee)
{
    scanner();

    if (token.type == SEMICOLON)
        return ERR_None;

    if (token.type == ASSIGNMENT)
        return (rule_expression(assignee));

    return ERR_SYNTAX;
}

int rule_expression(hashElem * assignee)
{
	/*scanner();
	if (strcmp(token.area, "__vyraz__") != 0)
		return ERR_SYNTAX;

	scanner();
	if (token.type != SEMICOLON)
		return ERR_SYNTAX;*/

	scanner();
	switch (token.type)
	{
		case IDENTIFIER:
			if (isFunct(token.area))
				return rule_funcCall();
			else
			{
				ungetToken();
				PrecedencniSA(tableStackTop(localSTstack));		//TODO pouzivat cely stack, treba na to vytvorit funkciu
				return ERR_None;
			}
			break;

		case B_LENGTH:
			return rule_BLength();

		case B_SUBSTR:
			return rule_BSubstr();

		case B_CONCAT:
			return rule_BConcat();

		case B_FIND:
			return rule_BFind();

		case B_SORT:
			return rule_BSort();

		default:
			ungetToken();
			PrecedencniSA(tableStackTop(localSTstack));		//TODO pouzivat cely stack, treba na to vytvorit funkciu
			return ERR_None;
	}

    PrecedencniSA(tableStackTop(localSTstack));			//TODO prsa.c sa musi naucit pouzivat tableStack
	return ERR_None;				//TODO	check error state
}

//rule:     <auto-decl> -> id = <expression>
int rule_auto()
{
    scanner();
    if (token.type != IDENTIFIER)
        return ERR_SYNTAX;

	hashElem * assignee;
	if ((assignee = isDeclaredOnTheSameLevel(token.area)) != NULL)
		return ERR_AttemptedRedefVar;

    scanner();
    if (token.type != ASSIGNMENT)
        return ERR_SYNTAX;

    return rule_expression(assignee);
} 

//rule:     <cin> -> >> id
int rule_cin()
{
    if (token.type != C_IN)
        return ERR_SYNTAX;

    scanner();
    if (token.type != IDENTIFIER)
        return ERR_SYNTAX;

	if (!findVar(token.area))
		return ERR_UndefinedVariable;

    return ERR_None;
}

// <cin-list> -> <cin> <cin-list>   ||  ;
int rule_cinList()
{
    scanner();

    if (token.type == SEMICOLON)
        return ERR_None;
    else
    {
        int error = rule_cin();
        if (error != ERR_None)
            return error;
        else 
            return (rule_cinList());
    }
}

//rule:     <cout> -> << id
int rule_cout()
{
    if (token.type != C_OUT)
        return ERR_SYNTAX;

    scanner();
    /*if ((token.type != IDENTIFIER) && (token.type != STRING) && (token.type != INT_NUMBER) && (token.type != DOUBLE_NUMBER))
        return ERR_SYNTAX;*/

	switch (token.type)
	{
		case STRING:
		case INT_NUMBER:
		case DOUBLE_NUMBER:
			break;

		case IDENTIFIER:
			if (!findVar(token.area))
				return ERR_UndefinedVariable;
			break;

		default:
			return ERR_SYNTAX;
	}

    return ERR_None;
}

// <cout-list> -> <cout> <cout-list>   ||  ;
int rule_coutList()
{
    scanner();

    if (token.type == SEMICOLON)
        return ERR_None;
    else
    {
        int error = rule_cout();
        if (error != ERR_None)
            return error;
        else 
            return (rule_coutList());
    }
}

//rule:     <if-decl> -> ( <expression> ) <st-list> else <st-list>
int rule_if()
{
    scanner();
    if (token.type != L_BRACKET)
        return ERR_SYNTAX;

    int error = rule_expression(NULL);					//TODO
    if (error != ERR_None)
        return error;

    scanner();
    if (token.type != R_BRACKET)
        return ERR_SYNTAX;

    scanner();
    if ((error = rule_statement()) != ERR_None)
        return error;

    scanner();
    if (token.type != K_ELSE)
    {
        ungetToken();
        return ERR_None;
    }
    
    scanner();
    if ((error = rule_statement()) != ERR_None)
        return error;

    return ERR_None;
}

//rule:     <return> -> expression
int rule_return()
{
    return rule_expression(NULL);				//TODO
}

//rule:     <for-decl> -> ( id <var-decl> <expression> id = <expression> ) { <st-list>
int rule_for()
{
    scanner();
    if (token.type != L_BRACKET)
        return ERR_SYNTAX;

	/*hTab * temp;
	if ((temp = hTabInit(INIT_ST_SIZE)) == NULL)
		return ERR_AllocFailed;

	if (tableStackPush(localSTstack, temp) == NULL)
	{
		hTabFree(temp);
		return ERR_AllocFailed;
	}*/    // NOT YET ; _ ;

    scanner();
    if ((token.type != K_INT) && (token.type != K_DOUBLE) && (token.type != K_STRING))
        return ERR_SYNTAX;

    scanner();
    if (token.type != IDENTIFIER)
        return ERR_SYNTAX;

    int error;

    if ((error = rule_varDecl(NULL)) != ERR_None)
        return error;

    if ((error = rule_expression(NULL)) != ERR_None)
        return error;

    scanner();
    if (token.type != IDENTIFIER)
        return ERR_SYNTAX;

    scanner();
    if (token.type != ASSIGNMENT)
        return ERR_SYNTAX;

    if ((error = rule_expression(NULL)) != ERR_None)			//rovno prsa namiesto expression?
        return error;

    scanner();
    if (token.type != R_BRACKET)
        return ERR_SYNTAX;

    scanner();
    error = rule_statement();

	return error;
}


//rule:     <keyword> -> auto <auto-decl>    ||      cin <cin> <cin-list>    ||  cout <cout> <cout-list>     ||   for <for-decl>  ||  if <if-decl>    ||  return <expression>

int rule_keyword()
{
    int error;

    switch (token.type)
    {
        case K_INT:
        case K_DOUBLE:
        case K_STRING:
        {
			int type = getType(token.type);
			scanner();
			
			if (token.type != IDENTIFIER)
				return ERR_SYNTAX;
			
			if (isDeclaredOnTheSameLevel(token.area) != NULL)
				return ERR_AttemptedRedefVar;

			if (findElem(globalST, token.area) != NULL)
				return ERR_AttemptedRedefVar;

			hashElem * temp;
			if ((temp = addVar(token.area, tableStackTop(localSTstack), type)) == NULL)
				return ERR_AllocFailed;
			
			return rule_varDecl(temp);
        }

        case K_CIN:
        {
            scanner();
            if ((error = rule_cin()) != ERR_None)
                return error;
            else 
                return rule_cinList();
        }

        case K_COUT:
        {
            scanner();
    
            if ((error = rule_cout()) != ERR_None)
                return error;
            else 
                return rule_coutList();
        }
    
        case K_IF:
            return rule_if();
    
        case K_FOR:
            return rule_for();
    
        case K_WHILE:
            return rule_while();
    
        case K_DO:
            return rule_do();
    
        case K_RETURN:
            return rule_return();
    
        case K_AUTO:
            return rule_auto();
    
        default:
            return ERR_SYNTAX;
    }
}

//rule:      <while-loop> -> ( <expression> ) <statement>
int rule_while()
{
    scanner();
    if (token.type != L_BRACKET)
        return ERR_SYNTAX;

    int error;
    if ((error = rule_expression(NULL)) != ERR_None)
        return error;

    scanner();
    if (token.type != R_BRACKET)
        return ERR_SYNTAX;

    scanner();
    if ((error = rule_statement()) != ERR_None)
        return error;

    //generovanie skoku and stuff

    return ERR_None;
}

//rule:     <do-loop> -> <statement> while ( <expression> );
int rule_do()
{
    int error;
    
    
    scanner();
    if ((error = rule_statement()) != ERR_None)
        return error;

    scanner();
    if (token.type != K_WHILE)
        return ERR_SYNTAX;

    scanner();
    if (token.type != L_BRACKET)
        return ERR_SYNTAX;

    if ((error = rule_expression(NULL)) != ERR_None)
        return error;

    scanner();
    if (token.type != R_BRACKET)
        return ERR_SYNTAX;

    scanner();
    if (token.type != SEMICOLON)
        return ERR_SYNTAX;

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

/*char * appendChar(char * string, char c)
{
    if (string == NULL)
    {
        string = malloc(2 * sizeof(char));
        if (string == NULL)
            return NULL;
        
        string[0] = c;
        string[1] = '\0';
    }

    else
    { 
        char * temp = realloc (string, (strlen(string) + 2) * sizeof(char) );
        if (temp == NULL)
            return NULL;

        string = temp;
        unsigned long a = strlen(string);
        string[a] = c;
        string[a+1] = '\0';
    }

    return string;
}*/

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
		if (!convertType(elem1->data.params[i].type, elem2->data.params[i].type))//(elem1->data.params[i].type != elem2->data.params[i].type)
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
	/*if ((temp = findElem(localST, key)) == NULL)		// the symbol wasn't found
		return 0;*/

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
	tParam * temp = realloc(elem->data.params, (elem->data.numberOfParams + 1) * sizeof(tParam));
	if (temp == NULL)
		return NULL;

	elem->data.params = temp;
	elem->data.params[elem->data.numberOfParams].type = type;

	if ((elem->data.params[elem->data.numberOfParams].key = strdup(key)) == NULL)
		return NULL;

	++elem->data.numberOfParams;
	return temp;
}

hashElem * isFunct(char * key)
{
	return findElem(globalST, key);
}

int rule_funcCall()	//id (<call_list>;
{
	hashElem funcCall;
	funcCall.key = NULL;
	funcCall.data.params = NULL;
	hashElemInit(&funcCall);

	funcCall.key = strdup(token.area);
	if (funcCall.key == NULL)
		return ERR_AllocFailed;

	scanner();
	if (token.type != L_BRACKET)
    {
        free(funcCall.key);
		return ERR_SYNTAX;
    }
    
	int error = rule_callParam(&funcCall);

	if (error != ERR_None)
    {
        free(funcCall.key);
        free(funcCall.data.params);
		return error;
    }

	if (!compareParamTypes(&funcCall, findElem(globalST, funcCall.key)))
		return ERR_ParamType;
	
	scanner();
	if (token.type != SEMICOLON)
		return ERR_SYNTAX;

	hashElemInit(&funcCall);
	return ERR_None;
}

int rule_callParam(hashElem * funcCall)
{
	scanner();

	hashElem * temp;	
	switch (token.type)
	{
		case R_BRACKET:
			return ERR_None;

		case IDENTIFIER:
			if ((temp = findVar(token.area)) == NULL)
				return ERR_UndefinedVariable;

			if (addParam(funcCall, "", temp->data.type) == NULL)
				return ERR_AllocFailed;
			return rule_callParamList(funcCall);

		case INT_NUMBER:
			if (addParam(funcCall, "", VAR_INT) == NULL)
				return ERR_AllocFailed;
			return rule_callParamList(funcCall);

		case DOUBLE_NUMBER:
			if (addParam(funcCall, "", VAR_DOUBLE) == NULL)
				return ERR_AllocFailed;
			return rule_callParamList(funcCall);

		case STRING:
			if (addParam(funcCall, "", VAR_STRING) == NULL)
				return ERR_AllocFailed;
			return rule_callParamList(funcCall);

		default:
			return ERR_SYNTAX;
	}
}

int rule_callParamList(hashElem * funcCall)
{
	scanner();
	switch (token.type)
	{
		case COMMA:
			break;

		case R_BRACKET:
			return ERR_None;

		default:
			return ERR_SYNTAX;
	}

	hashElem * temp;
	scanner();
	switch (token.type)
	{
		case IDENTIFIER:
			if ((temp = findVar(token.area)) == NULL)
				return ERR_UndefinedVariable;

			if (addParam(funcCall, "", temp->data.type) == NULL)
				return ERR_AllocFailed;
			return rule_callParamList(funcCall);

		case INT_NUMBER:
			if (addParam(funcCall, "", VAR_INT) == NULL)
				return ERR_AllocFailed;
			return rule_callParamList(funcCall);

		case DOUBLE_NUMBER:
			if (addParam(funcCall, "", VAR_DOUBLE) == NULL)
				return ERR_AllocFailed;
			return rule_callParamList(funcCall);

		case STRING:
			if (addParam(funcCall, "", VAR_STRING) == NULL)
				return ERR_AllocFailed;
			return rule_callParamList(funcCall);

		default:
			return ERR_SYNTAX;
	}	
}

int rule_BLength()
{
	scanner();
	if (token.type != L_BRACKET)
		return ERR_SYNTAX;

	scanner();

	switch (token.type)
	{
		case STRING:
			break;
		
		case IDENTIFIER:
			if (findVar(token.area)->data.type == VAR_STRING)
				break;
			else
				return ERR_ParamType;
		
		case R_BRACKET:
			return ERR_ParamNumber;
		
		default:
			return ERR_SYNTAX;
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
					return ERR_ParamNumber;
				default:
					return ERR_SYNTAX;
			}

		default:
			return ERR_SYNTAX;
	}

	scanner();
	switch (token.type)
	{
		case SEMICOLON:
			return ERR_None;

		default:
			return ERR_SYNTAX;
	}
}

int rule_BSubstr()
{
	/*scanner();
	if (token.type != L_BRACKET)
		return ERR_SYNTAX;

	scanner();

	switch (token.type)
	{
	case STRING:
		break;

	case IDENTIFIER:
		if (findVar(token.area)->data.type == VAR_STRING)
			break;
		else
			return ERR_ParamType;

	case R_BRACKET:
		return ERR_ParamNumber;

	default:
		return ERR_SYNTAX;
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
			return ERR_ParamNumber;
		default:
			return ERR_SYNTAX;
		}

	default:
		return ERR_SYNTAX;
	}

	scanner();
	switch (token.type)
	{
	case SEMICOLON:
		return ERR_None;

	default:
		return ERR_SYNTAX;
	}*/

	return ERR_None;
}

int rule_BConcat()
{
	return ERR_None;
}

int rule_BFind()
{
	return ERR_None;
}

int rule_BSort()
{
	scanner();
	if (token.type != L_BRACKET)
		return ERR_SYNTAX;				//Či?

	scanner();

	switch (token.type)
	{
	case STRING:
		break;

	case IDENTIFIER:
		if (findVar(token.area)->data.type == VAR_STRING)
			break;
		else
			return ERR_ParamType;

	case R_BRACKET:
		return ERR_ParamNumber;

	default:
		return ERR_SYNTAX;
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
			return ERR_ParamNumber;
		default:
			return ERR_SYNTAX;
		}

	default:
		return ERR_SYNTAX;
	}

	scanner();
	switch (token.type)
	{
	case SEMICOLON:
		return ERR_None;

	default:
		return ERR_SYNTAX;
	}
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
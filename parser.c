#include "parser.h"


FILE* srcFile;

int isKeyword(int tokenType)
{
    return (tokenType >= K_AUTO) && (tokenType <= K_DO);
}

int parse(FILE *source)
{
    srcFile = source;

    hashElem activeElem;
    activeElem.data.fParamTypes = NULL;
    activeElem.data.localTable = NULL;
    activeElem.key = NULL;

    globalST = hTabInit(INIT_ST_SIZE);

    int returnValue = rule_funcdef(&activeElem);
    
    //cleanup?

    return returnValue;
}

// rule:    <prog> -> type id <param-list> <func-defined>   || EOF
int rule_funcdef(hashElem * activeElem)
{
    hashElemInit(activeElem);

    scanner(srcFile);
    if (token.type == EOF)
        return ERR_None;

    if ((token.type != K_INT) && (token.type != K_DOUBLE) && (token.type != K_STRING))
        return ERR_SYNTAX;


    activeElem->data.type = func;
    activeElem->data.state = declared;

    activeElem->data.fParamTypes = appendChar(activeElem->data.fParamTypes, paramTypeToChar(token.type));    //set function type
    
    scanner(srcFile);
    if (token.type != IDENTIFIER)
        return ERR_SYNTAX;

    if ((activeElem->key = malloc((strlen(token.area) + 1) * sizeof(char))) == NULL)
        return ERR_AllocFailed;
    strcpy(activeElem->key, token.area);                                     //save the function name
    
    int error = rule_paramList(activeElem);
    if (error != ERR_None)
        return error;

    if (!strcmp(activeElem->key, "main") && (strcmp(activeElem->data.fParamTypes, "i")))      //main should be of type int and with no parameters
    {
        return ERR_UndefinedFunction;
    }

    error = rule_funcDefined(activeElem);
    if (error != ERR_None)
        return error;

    return rule_funcdef(activeElem);
}

//rule:     <func-defined> -> { <st-list>        ||      ; 
int rule_funcDefined(hashElem * activeElem)
{
    printf("Funkcia %s parametre: %s\n", activeElem->key, activeElem->data.fParamTypes);

    hashElem * temp = findElem(globalST, activeElem->key);                           //check whether there already is a function with given name

    if (temp != NULL)
        printf("Najdena funkcia %s parametre: %s\n", temp->key, temp->data.fParamTypes);

    if (temp == NULL)
    {                                                           //there's no such function declared in symbol table
            if ((temp = addElem(globalST, activeElem->key, activeElem->data)) == NULL)
                    return ERR_AllocFailed;
    }

    // the function has already been declared   
    else if (!compareSymbol(temp, activeElem))
        return ERR_AttemptedRedefFunction;

    scanner(srcFile);
    if (token.type == SEMICOLON)                                                    //the current function has been declared, not defined
        return ERR_None;

    else if (token.type != L_CURLY_BRACKET)
            return ERR_SYNTAX;

    activeElem->data.state = defined;

    if ((temp != NULL) && (temp->data.state == defined))
        return ERR_AttemptedRedefFunction;
    
   /* else if (temp == NULL)
        temp = addElem(globalST, activeElem->key, activeElem->data);*/

    temp->data.state = defined;

    if ((temp->data.localTable = hTabInit(INIT_ST_SIZE)) == NULL)           //init the functions local symbol table
        return ERR_AllocFailed;
    localST = temp->data.localTable;


    scanner(srcFile);
    int error = rule_stList();
    if (error != ERR_None)
        return error;

    return ERR_None;
}

//rule:     <param-list> -> ( <param> <param-next>      |       ()
int rule_paramList(hashElem * activeElem)
{
    scanner(srcFile);
    if (token.type != L_BRACKET)
        return ERR_SYNTAX;

    scanner(srcFile);
    if (token.type == R_BRACKET)
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
    if ((token.type != K_INT) && (token.type != K_DOUBLE) && (token.type != K_STRING))
        return ERR_SYNTAX;

    activeElem->data.fParamTypes = appendChar(activeElem->data.fParamTypes, paramTypeToChar(token.type));    //add param type

    scanner(srcFile);
    if (token.type != IDENTIFIER)
        return ERR_SYNTAX;

    //pridanie ID premennej do tabulky

    return ERR_None;
}

//rule:     <param-next> -> , <param> <param-next>      |   )
int rule_paramNext(hashElem * activeElem)
{
    scanner(srcFile);
    
    if (token.type == COMMA)
    {

        scanner(srcFile);
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
        return ERR_None;
    
    error = rule_statement();
        if (error != ERR_None)
            return error;

    scanner(srcFile);
    error = rule_stList();
        if (error != ERR_None)
            return error;

    return ERR_None;
}

//rule:     <statement> -> type id <var-decl> ;   ||      id <assignment> ;      ||        id (<param-list> ;       ||      <keyword>   || { <st-list>
int rule_statement()
{
    if (token.type == IDENTIFIER)
    {
        scanner(srcFile);
        if (token.type == ASSIGNMENT)
            return rule_expression();
    }

    else if (isKeyword(token.type))
    {
        return rule_keyword();
    }

    else if (token.type == L_CURLY_BRACKET)
    {
        scanner(srcFile);
        return rule_stList();
    }

    return ERR_SYNTAX;
}

//rule:     <var-decl> -> ;    |   = <expression> ;
int rule_varDecl()
{
    scanner(srcFile);

    if (token.type == SEMICOLON)
        return ERR_None;

    if (token.type == ASSIGNMENT)
        return (rule_expression());

    return ERR_SYNTAX;
}

int rule_expression()
{
    scanner(srcFile);
    if (strcmp(token.area, "__vyraz__") != 0)
        return ERR_SYNTAX;

    scanner(srcFile);
    if (token.type != SEMICOLON)
        return ERR_SYNTAX;

    return ERR_None;
}

//rule:     <auto-decl> -> id = <expression>
int rule_auto()
{
    scanner(srcFile);
    if (token.type != IDENTIFIER)
        return ERR_SYNTAX;

    scanner(srcFile);
    if (token.type != ASSIGNMENT)
        return ERR_SYNTAX;

    return rule_expression();
} 

//rule:     <cin> -> >> id
int rule_cin()
{
    if (token.type != C_IN)
        return ERR_SYNTAX;

    scanner(srcFile);
    if (token.type != IDENTIFIER)
        return ERR_SYNTAX;

    return ERR_None;
}

// <cin-list> -> <cin> <cin-list>   ||  ;
int rule_cinList()
{
    scanner(srcFile);

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

    scanner(srcFile);
    if ((token.type != IDENTIFIER) && (token.type != STRING) && (token.type != INT_NUMBER) && (token.type != DOUBLE_NUMBER))
        return ERR_SYNTAX;

    return ERR_None;
}

// <cout-list> -> <cout> <cout-list>   ||  ;
int rule_coutList()
{
    scanner(srcFile);

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
    scanner(srcFile);
    if (token.type != L_BRACKET)
        return ERR_SYNTAX;

    int error = rule_expression();
    if (error != ERR_None)
        return error;

    scanner(srcFile);
    if (token.type != R_BRACKET)
        return ERR_SYNTAX;

    scanner(srcFile);
    if ((error = rule_statement()) != ERR_None)
        return error;

    scanner(srcFile);
    if (token.type != K_ELSE)
    {
        ungetToken();
        return ERR_None;
    }
    
    scanner(srcFile);
    if ((error = rule_statement()) != ERR_None)
        return error;

    return ERR_None;
}

//rule:     <return> -> expression
int rule_return()
{
    return rule_expression();
}

//rule:     <for-decl> -> ( id <var-decl> <expression> id = <expression> ) { <st-list>
int rule_for()
{
    scanner(srcFile);
    if (token.type != L_BRACKET)
        return ERR_SYNTAX;

    scanner(srcFile);
    if ((token.type != K_INT) && (token.type != K_DOUBLE) && (token.type != K_STRING))
        return ERR_SYNTAX;

    scanner(srcFile);
    if (token.type != IDENTIFIER)
        return ERR_SYNTAX;

    int error;

    if ((error = rule_varDecl()) != ERR_None)
        return error;

    if ((error = rule_expression()) != ERR_None)
        return error;

    scanner(srcFile);
    if (token.type != IDENTIFIER)
        return ERR_SYNTAX;

    scanner(srcFile);
    if (token.type != ASSIGNMENT)
        return ERR_SYNTAX;

    if ((error = rule_expression()) != ERR_None)
        return error;

    scanner(srcFile);
    if (token.type != R_BRACKET)
        return ERR_SYNTAX;

    scanner(srcFile);
    return rule_statement();
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
                scanner(srcFile);
        
                if (token.type != IDENTIFIER)
                    return ERR_SYNTAX;
        
                return rule_varDecl();
        }

        case K_CIN:
        {
            scanner(srcFile);
            if ((error = rule_cin()) != ERR_None)
                return error;
            else 
                return rule_cinList();
        }

        case K_COUT:
        {
            scanner(srcFile);
    
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
    scanner(srcFile);
    if (token.type != L_BRACKET)
        return ERR_SYNTAX;

    int error;
    if ((error = rule_expression()) != ERR_None)
        return error;

    scanner(srcFile);
    if (token.type != R_BRACKET)
        return ERR_SYNTAX;

    scanner(srcFile);
    if ((error = rule_statement()) != ERR_None)
        return error;

    //generovanie skoku and stuff

    return ERR_None;
}

//rule:     <do-loop> -> <statement> while ( <expression> );
int rule_do()
{
    int error;
    
    
    scanner(srcFile);
    if ((error = rule_statement()) != ERR_None)
        return error;

    scanner(srcFile);
    if (token.type != K_WHILE)
        return ERR_SYNTAX;

    scanner(srcFile);
    if (token.type != L_BRACKET)
        return ERR_SYNTAX;

    if ((error = rule_expression()) != ERR_None)
        return error;

    scanner(srcFile);
    if (token.type != R_BRACKET)
        return ERR_SYNTAX;

    scanner(srcFile);
    if (token.type != SEMICOLON)
        return ERR_SYNTAX;

    return ERR_None;
}

int getType(int tokenType)
{
    switch (tokenType)
    {
        case K_INT:
            return var_int;

        case K_DOUBLE:
            return var_double;

        case K_STRING:
            return var_string;

        default:
            return -1;
    }
}

char paramTypeToChar(int type)
{
    switch (type)
    {
        case K_INT:
            return 'i';

        case K_DOUBLE:
            return 'd';

        case K_STRING:
            return 's';

        default:
            return -1;
    }
}

char * appendChar(char * string, char c)
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
}

int compareSymbol(hashElem * elem, hashElem * activeElem)                          //compares elem with global variable activeElem
{
    if (elem->data.type != activeElem->data.type)
        return 0;

    /*if ((elem->data.fParamTypes == NULL) || (activeElem->data.fParamTypes == NULL))
        return 1;*/

    if (strcmp(elem->data.fParamTypes, activeElem->data.fParamTypes) != 0)
     return 0;

    printf("Su rovnaké: --%s--%s--\n", elem->data.fParamTypes, activeElem->data.fParamTypes);

    return 1;
}

//
//  error.c
//

#include <stdio.h>
#include "error.h"

EErrorReturnValues getReturnValue (EErrorTypes value)
/*
 * Function computes appropriate return value of the program based on the type of error that has occured.
 */
{
    EErrorReturnValues returnValue = value/100;
    return returnValue;
}

void printErrorDetails (SError errorDetails)
/*
 * Prints information about the occured error - line number and appropriate message.
 */
{
    printf("Error on line number %d. \n", errorDetails.line);
    
    switch (errorDetails.state) {
        case ERR_LEXICAL:
            printf("");
            break;
        case ERR_SYNTAX:
            printf("");
            break;
        case ERR_UndefinedFunction:
            printf("The function has not been defined.");
            break;
        case ERR_AttemptedRedefFunction:
            printf("Attempted redifinition of function.");
            break;
        case ERR_AttemptedRedefVar:
            printf("Attempted redefinition of variable.");
            break;
        case ERR_IncompatibleExpr:
            printf("Incompatible types of expressions.");
            break;
        case ERR_ParamNumber:
            printf("Incorrect number of parameters.");
            break;
        case ERR_ParamType:
            printf("Incorrect type of parameter.");
            break;
        case ERR_UnderivableType:
            printf("The type of variable cannot be derived.");
            break;
        case ERR_AutoNoInit:
            printf("Using \'auto\', but initialization is missing.");
            break;
        case ERR_OTHERSEMANTIC:
            printf("");
            break;
        case ERR_ReadInput:
            printf("Interpreter error while reading from input.");
            break;
        case ERR_UninitVar:
            printf("Interpreter error while working with uninicialized variable.");
            break;
        case ERR_ZeroDivision:
            printf("Zero division.");
            break;
        case ERR_OtherRunningErr:
            printf("");
            break;
        case ERR_AllocFailed:
            printf("Memory allocation error.");
            break;
        case ERR_CannotOpenFile:
            printf("File cannot be opened.");
            break;
        case ERR_InvalidCommndLineArgs:
            printf("Invalid command line arguments.");
            break;
        default:
            break;
    }
}

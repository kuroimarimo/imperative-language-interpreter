//
//  error.c
//

#include <stdio.h>
#include <stdlib.h>
#include "error.h"

EErrorReturnValues getReturnValue (EErrorTypes value)
/*
 * The function computes the appropriate return value of the program based on the type of error that has occurred.
 */
{
    EErrorReturnValues returnValue = value/100;
    return returnValue;
}

void printErrorDetails (SError errorDetails)
/*
 * Prints information about the occurred error - line number and appropriate message.
 */
{
    fprintf(stderr, "Error on line number %d. \n", errorDetails.line);
    
    switch (errorDetails.state) {
        case ERR_LEXICAL:
            fprintf(stderr, "");
            break;
        case ERR_SYNTAX:
            fprintf(stderr, "");
            break;
        case ERR_UndefinedFunction:
            fprintf(stderr, "The function has not been defined.");
            break;
        case ERR_AttemptedRedefFunction:
            fprintf(stderr, "Attempted redefinition of function.");
            break;
        case ERR_AttemptedRedefVar:
            fprintf(stderr, "Attempted redefinition of variable.");
            break;
        case ERR_IncompatibleExpr:
            fprintf(stderr, "Incompatible types of expressions.");
            break;
        case ERR_ParamNumber:
            fprintf(stderr, "Incorrect number of parameters.");
            break;
        case ERR_ParamType:
            fprintf(stderr, "Incorrect type of parameter.");
            break;
        case ERR_UnderivableType:
            fprintf(stderr, "The type of variable cannot be derived.");
            break;
        case ERR_AutoNoInit:
            fprintf(stderr, "Using \'auto\', but initialization is missing.");
            break;
        case ERR_OTHERSEMANTIC:
            fprintf(stderr, "");
            break;
        case ERR_ReadInput:
            fprintf(stderr, "Interpreter error while reading from the input.");
            break;
        case ERR_UninitVar:
            fprintf(stderr, "Interpreter error while working with uninitialized variable.");
            break;
        case ERR_ZeroDivision:
            fprintf(stderr, "Zero division.");
            break;
        case ERR_OtherRunningErr:
            fprintf(stderr, "");
            break;
        case ERR_AllocFailed:
            fprintf(stderr, "Memory allocation error.");
            break;
        case ERR_CannotOpenFile:
            fprintf(stderr, "File cannot be opened.");
            break;
        case ERR_InvalidCommndLineArgs:
            fprintf(stderr, "Invalid command line arguments.");
            break;
        default:
            break;
    }
}

void fatalError (SError error)
{
    printErrorDetails (error);
    exit (getReturnValue(error.state));
}

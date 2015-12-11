//
//  error.c
//

#include "error.h"

EErrorReturnValues getReturnValue (EErrorTypes value)
/*
 * The function computes the appropriate return value of the program based on the type of error that has occurred.
 */
{
    EErrorReturnValues returnValue = value/100;
    return returnValue;
}

void printErrorDetails (EErrorTypes state)
/*
 * Prints information about the occurred error - line number and appropriate message.
 */
{
    fprintf(stderr, "\nError on line number %d. \n", token.counter_of_lines);

    switch (state) {
        case ERR_LEXICAL:
            fprintf(stderr, "Lexical error\n");
            break;
        case ERR_NumberShape:
            fprintf(stderr, "Number is in wrong shape.\n");
            break;
        case ERR_NumberEscape:
            fprintf(stderr, "Number escape is in wrong shape.\n");
            break;
        case ERR_StringEscape:
            fprintf(stderr, "Error escaping string.\n");
            break;
        case ERR_StringChar:
            fprintf(stderr, "Error invlid character.\n");
            break;
        case ERR_UnknownChar:
            fprintf(stderr, "Unknown character.\n");
            break;
        case ERR_Comment:
            fprintf(stderr, "Unterminated comment.\n");
            break;
        case ERR_SYNTAX:
            fprintf(stderr, "Syntax error.\n");
            break;
        case ERR_UndefinedFunction:
            fprintf(stderr, "The function has not been defined.\n");
            break;
        case ERR_AttemptedRedefFunction:
            fprintf(stderr, "Attempted redefinition of function.\n");
            break;
        case ERR_AttemptedRedefVar:
            fprintf(stderr, "Attempted redefinition of variable.\n");
            break;
		case ERR_UndefinedVariable:
			fprintf(stderr, "The variable has not been defined.\n");
			break;
        case ERR_IncompatibleExpr:
            fprintf(stderr, "Incompatible types of expressions.\n");
            break;
        case ERR_ParamNumber:
            fprintf(stderr, "Incorrect number of parameters.\n");
            break;
        case ERR_ParamType:
            fprintf(stderr, "Incorrect type of parameter.\n");
            break;
        case ERR_UnderivableType:
            fprintf(stderr, "The type of variable cannot be derived.\n");
            break;
        case ERR_AutoNoInit:
            fprintf(stderr, "Using \'auto\', but initialization is missing.\n");
            break;
        case ERR_OTHERSEMANTIC:
            fprintf(stderr, "Semantic error.\n");
            break;
        case ERR_ReadInput:
            fprintf(stderr, "Interpreter error while reading from the input.\n");
            break;
        case ERR_UninitVar:
            fprintf(stderr, "Interpreter error while working with uninitialized variable.\n");
            break;
        case ERR_ZeroDivision:
            fprintf(stderr, "Zero division.\n");
            break;
        case ERR_OtherRunningErr:
            fprintf(stderr, "Runtime error.\n");
            break;
        case ERR_SegmentationFault:
            fprintf(stderr, "Segmentation fault.\n");
            break;
        case ERR_AllocFailed:
            fprintf(stderr, "Memory allocation error.\n");
            break;
        case ERR_CannotOpenFile:
            fprintf(stderr, "File cannot be opened.\n");
            break;
        case ERR_InvalidCommndLineArgs:
            fprintf(stderr, "Invalid command line arguments.\n");
            break;
        default:
			fprintf(stderr, "Undefined error.\n");
            break;
    }
}

void fatalError (EErrorTypes state)
{
    printErrorDetails (state);
	garbageCleanup(&garbageList);
	fclose(source);
    exit(getReturnValue(state));
}

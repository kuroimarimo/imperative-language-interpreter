/*
*  Project name:
*  Implementace interpretu imperativního jazyka IFJ15
*
*  Date: 13.12.2015
*
*  Repository:
*  https://github.com/kuroimarimo/imperative-language-interpreter
*
*  Team:
*  Votjěch Václavík	(xvacla22)
*  Peter Vančo			(xvanco05)
*  Filip Vaško         (xvasko10)
*  Dominik Vašek		(xvasek06)
*  Valentína Straková	(xstrak27)
*/

#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdlib.h>

#include "garbage_collector.h"
#include "lex_an.h"

typedef enum
{
	ERR_None,

	//// LEXIS
	ERR_LEXICAL				= 100,
	ERR_NumberShape,
	ERR_StringEscape,
	ERR_NumberEscape,
	ERR_StringChar,
	ERR_UnknownChar,
	ERR_Comment,

	//// SYNTAX
	ERR_SYNTAX				= 200,
	ERR_DATA_TYPE_EXPECTED,
	ERR_ExpectedIdentifier,
	ERR_FuncDeclEnd,
	ERR_LBracket,
	ERR_SyntaxParam,
	ERR_SxStatement,
	ERR_SxAuto,
	ERR_SxCout,
	ERR_SxCin,
	ERR_ExpectedAssignment,
	ERR_ExpectedSemicol,
	ERR_ExpectedParameter,

    //// SEMANTICS
    // semantic errors in program
    ERR_UndefinedFunction   = 300,
    ERR_AttemptedRedefFunction,
    ERR_AttemptedRedefVar,
    ERR_UndefinedVariable,

    // semantic type compatibility errors
    ERR_IncompatibleExpr    = 400,
    ERR_ParamNumber,
    ERR_ParamType,

    // type derivation errors
    ERR_UnderivableType     = 500,
    ERR_AutoNoInit,

    // other semantic errors
    ERR_OTHERSEMANTIC       = 600,              // TODO prepisat ked budeme mat konkretne chybove stavy
    ERR_NFunction,                              // precedence analysis is unable to process function call, syntax has to handle it
	ERR_NCondition,

    //// INTERPRETER
    ERR_ReadInput           = 700,
    ERR_UninitVar           = 800,
	ERR_NoReturn,
    ERR_ZeroDivision        = 900,
    ERR_OtherRunningErr     = 1000,            // TODO prepisat ked budeme mat konkretne chybove stavy
    ERR_SegmentationFault,

    //// INTERNAL INTERPRETER ERRORS
    ERR_AllocFailed         = 9900,
    ERR_CannotOpenFile,
    ERR_InvalidCommndLineArgs,
} EErrorTypes;

typedef enum
{
    ERR_RV_None,
    ERR_RV_Lexical,             // RV = Return Value
    ERR_RV_Syntactic,
    ERR_RV_SemanticProgram,
    ERR_RV_SemanticCompatibility,
    ERR_RV_TypeDerivation,
    ERR_RV_OtherSemantic,
    ERR_RV_ReadInput,
    ERR_RV_UninitVar,
    ERR_RV_ZeroDivision,
    ERR_RV_OtherRunningErr,
    ERR_RV_InternalInterpretErr = 99,
} EErrorReturnValues;

typedef struct
{
    EErrorTypes state;          // what kind of error has occurred
    unsigned line;              // on what line of code
} SError;

//SError errorState;

EErrorReturnValues getReturnValue (EErrorTypes value);

void printErrorDetails (EErrorTypes state);

void fatalError (EErrorTypes state);

#endif

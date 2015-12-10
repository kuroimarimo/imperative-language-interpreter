#ifndef AK_H
#define AK_H

enum adressCode {
	OP_ASSIGN = 0,			// prirazeni in1 -> out
	OP_SUM,					// secteni in1, in2 -> out
	OP_MINUS,				// odecteni in1, in2 -> out
	OP_MUL,					// nasobeni in1, in2 -> out
	OP_DIV,					// deleni in1, in2 -> out
	OP_MOD,					// modulo in1, in2 -> out
	OP_LT,					// in1 mensi nez in2 -> out
	OP_GT,					// in1 vetsi nez in2 -> out
	OP_LE,					// in1 mensirovno in2 -> out
	OP_GE,					// in1 vetsirovno in2 -> out
	OP_EQUALS,				// in1 rovna se in2 -> out
	OP_DIFF,				// in1 ruzne od in2 -> out
	OP_INT,					// aloc. 4 bajtu -> out
	OP_DOUBLE,				// aloc 8 bajtu -> out
	OP_STRING,				// aloc x bajtu -> out
	OP_COUT,				// >> dalsi COUT, vystup -> out
	OP_CIN,					// store stdin input in *tVarCoordinates output
	OP_IF,					// pokud plati in1 -> out
	OP_ELSE, 				// pokud neplati in1 -> out
	OP_LABEL,				// in1 <- navesti pro goto
	OP_GOTO,				// skok pro for -> out
	OP_RETURN,				// converts input1 to type input2 and stores the result somewhere //TODO where?
	
	
	
	OP_CREATE_VAR,			// create variable of type *(tSymbolType*)input1 at *tVarCoordinates output
	OP_CREATE_FRAME,		// create variable frame of size *(int*)input1
	OP_SET_TOP_TO_BASE,		// changes the top frame to the base frame
	OP_DISPOSE_FRAME,		// dispose of the frame at the top of the framestack
	OP_IF_JUMP,				// if input1, jump to *tInstruction output
	OP_IFNOT_JUMP,			// if not input1, jump to *tInstruction output
	OP_FUNC_CALL,			// call a function, starting on *tInstruction input1
	OP_BUILT_IN,			// calls built in function of type input1 (from lex_an.h), stores the result in *tVarCoordinates output
	OP_GET_RETURN_VALUE,	// copies function return value to output 
	OP_NOP					// doesn't do anything
}; 

#endif

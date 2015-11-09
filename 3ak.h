enum 3ak {
OP_ASSIGN = 0,	// prirazeni in1 -> out
OP_SUM,			// secteni in1, in2 -> out
OP_MINUS,		// odecteni in1, in2 -> out
OP_MUL,			// nasobeni in1, in2 -> out
OP_DIV,			// deleni in1, in2 -> out
OP_MOD,			// modulo in1, in2 -> out
OP_LT,			// in1 mensi nez in2 -> out
OP_GT,			// in1 vetsi nez in2 -> out
OP_LE,			// in1 mensirovno in2 -> out
OP_GE,			// in1 vetsirovno in2 -> out
OP_EQUALS,		// in1 rovna se in2 -> out
OP_DIFF,		// in1 ruzne od in2 -> out
OP_INT,			// aloc. 4 bajtu -> out
OP_DOUBLE,		// aloc 8 bajtu -> out
OP_STRING,		// aloc x bajtu -> out
OP_COUT,		// >> dalsi COUT, vystup -> out
OP_CIN,			// << dalsi CIN, vstup -> out
OP_IF,			// pokud plati in1 -> out
OP_ELSE, 		// pokud neplati in1 -> out
OP_LABEL,		// in1 <- navesti pro goto
OP_GOTO,		// skok pro for -> out
OP_RETURN		// in1 ulozi -> out
}; 

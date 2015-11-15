// TODO
/*NextToken()	// pseudo !!!!!!!!!!!
lex get token
if token == +,-,*,/,==,!=,<,<=,>=,>,(,)
	then type = operator
	else identifikator
if type = operator
	then token = i;
-----------------
error propojka

return prec.SA???
*/
#include "prsa.h"

//--------------------------//
//		ZASOBNIK			//
//--------------------------//
void Init (tList *Z) {											// inicializace
	Z->First = NULL;
}

void Dispose (tList *Z) {
	tElemPtr ukazatel = NULL;									// deklarace pro predani ukazatele	
	while(Z->First != NULL) {									// cyklus pro free() zasobniku
		ukazatel = Z->First->ptr;
		free(Z->First);
		Z->First = ukazatel;
	}
	Init(Z);													// reinicializace zasobniku
}

void FirstTerminal (tList *Z, int *val) {
	if (Z->First != NULL) {
		*val = Z->First->data;
		while (val == 'E') 										// hledam terminal
			if (Z->First->ptr != NULL) 
				*val = Z->First->data;							// aktualni terminal
			//else error();
	}
	//else error
}

void Push (tList *Z, int val) {
	tElemPtr ukazatel = NULL;
	if(Z->First != NULL) ukazatel = Z->First;
	tElemPtr struktura = malloc(sizeof(struct tElem));			// alokace struktury
	if(struktura != NULL) {										// kontrola alokace
	 	Z->First = struktura;									// prirazeni a naplneni
	 	struktura->data = val;
	 	struktura->ptr = ukazatel;
	}
	//else error
}

void Pop (tList *Z, int *val) {
	if (Z->First != NULL) {
		*val = Z->First->data;									// pop hodnoty
		tElemPtr ukazatel = NULL; 								// deklarace pro presun ukazatelu
		if (Z->First->ptr != NULL) ukazatel = Z->First->ptr; 	// uchovani ukazatele
		free(Z->First);
		Z->First = ukazatel;									// posunuti fronty
	}
	//else error
}

//--------------------------//
//		GRAMATIKA			//
//--------------------------//
int PrecG(int suc1, int suc2, int suc3) {
switch (suc1) {
	case 'E':
		if (suc3 == 'E') {
			if (suc2 == PLUS) {					// pro E -> E+E
				printf("E -> E+E\n");
				return 1; 							// return 1
			}
			if (suc2 == MINUS) {				// pro E -> E-E
				printf("E -> E-E\n");
				return 2;							// return 2 
			}
			if (suc2 == MULTIPLY) {				// pro E -> E*E
				printf("E -> E*E\n");
				return 3; 							// return 3
			}
			if (suc2 == DIVIDE) {				// pro E -> E/E
				printf("E -> E/E\n");
				return 4; 							// return 4
			}
			if (suc2 == GREATER) {				// pro E -> E>E
				printf("E -> E>E\n");
				return 5;							// return 5
			}
			if (suc2 == GREATER_ROVNO) {		// pro E -> E>=E
				printf("E -> E>=E\n");
				return 6;							// return 6
			}
			if (suc2 == LESS) {					// pro E -> E<E
				printf("E -> E<E\n");
				return 7;							// return 7
			}
			if (suc2 == LESS_ROVNO) {			// pro E -> E<=E
				printf("E -> E<=E\n");
				return 8;							// return 8
			}
			if (suc2 == EQUAL) {				// pro E -> E==E
				printf("E -> E==E\n");
				return 9;							// return 9
			}
			if (suc2 == NEGACE) {				// pro E -> E!=E
				printf("E -> E!=E\n");
				return 10;							// return 10
			}
		}
		break;
	case 'i':									// pro E -> i
		printf("E -> i\n");
		return 11;									// return 11
	case L_BRACKET:								// pro E -> (E)
		if (suc2 == E) {
			if (suc3 == R_BRACKET) {
				printf("E -> (E)\n");
				return 12;							// return 12
			}
		}
		break;
	}
	return 0;
}

//------------------------------//
//	PRECEDENCNI SYNTAX ANALYZA	//
//------------------------------//
int PrecedencniSA () {
	int val, top, input, suc1, suc2, suc3;				// promenne
	tList *Z;											// zasobnik
	Init(Z);
	Push(Z, SEMICOLON);
	NextToken(&input);									// input
	FirstTerminal(Z, &top);								// nacteni pocatku zasobniku
	while (top != SEMICOLON && input != SEMICOLON) {	// dokud není vstup i zasobnik prazdny
		FirstTerminal(Z, &top);							// nacti dalsi terminal
		switch (precedencni_tabulka[OperatorToIndex(top)][OperatorToIndex(input)])	// najdi pravidlo v prec. tabulce
			case '=': 									// pokud se rovnaji
				push(Z, input);							// zapis do zasobniku
				input = NextToken();					// nacti dalsi znak na vstupu
				break;
			case '<': 									// pokud ma zasobnik mensi prioritu nez vstup (dle tabulky)
				Push(Z, '<');							// zapis do zasobniku znak <
				Push(Z, input);							// zapis vstup
				input = NextToken();					// nacti dalsi vstup
				break;
			case '>':									// pokud ma zasobnik vetsi prioritu nez vstup (dle tabulky)
				do {
					Pop(Z, top);						// vytahni prvek na top
					suc3 = suc2;						// zapis 3. predchudce
					suc2 = suc1;						// zapis 2. predchudce
					suc1 = top;							// zapis prvniho predchudce
				} while (top != '<')					// dokud v promennych nemam cely retezec, pro ktery hledam pravidlo
				if (top == '<' && PrecG(suc1, suc2, suc3) != 0) Push(Z, 'E');	// kontrola na nalezene pravidlo, pokud existuje, prepise na E -> neterminal
				else error();
				break;
			case ' ': 									// chyba ve vyrazu
				error();
				break;
	}
	Dispose(Z);											// uvolneni zasobniku
	printf("JOOO\n");									// radostne povzdechnuti
}

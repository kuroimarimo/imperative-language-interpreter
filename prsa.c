/*
-----------------
error propojka

return prec.SA ukazatel na výsledek; return SA;
*/
#include "prsa.h"

tExpr* NextToken() {
	FILE* srcFile;
	scanner(srcFile);
	char* str;
	tExpr* struktura;
	if ((struktura = malloc(sizeof(tExpr))) == NULL)
		return error; // chyba
	struktura->terminal = TERMINAL;
	struktura->type = token.type;
	if (token.type == INT_NUMBER) {
		if ((struktura->data = malloc(sizeof(int))) == NULL)
			return error;
		*struktura->data = token.int_numb;
	}
	else if (token.type == DOUBLE_NUMBER) {
		if ((struktura->data = malloc(sizeof(double))) == NULL)
			return error;
		*struktura->data = token.double_numb;
	}
	else if (token.type == STRING || token.type == IDENTIFIER) {	
		if ((str = malloc(strlen(token.area)+1)) == NULL)
			return error; // chyba
		struktura->data = str;
		strcpy(struktura->data, token.area);
	}
	return struktura;
}
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
tExpr* FirstTerminal (tList *Z) {
	tExpr* val = NULL;
	if (Z->First != NULL) {
		val = Z->First->data;
		while (val.terminal == NETERMINAL) 										// hledam terminal
			if (Z->First->ptr != NULL) 
				val = Z->First->data;							// aktualni terminal
			//else error();
	}
	return val;
}
void Push (tList *Z, tExpr *val) {
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
tExpr* Pop (tList *Z) {
	tExpr* val = NULL;
	if (Z->First != NULL) {
		val = Z->First->data;									// pop hodnoty
		tElemPtr ukazatel = NULL; 								// deklarace pro presun ukazatelu
		if (Z->First->ptr != NULL) ukazatel = Z->First->ptr; 	// uchovani ukazatele
		free(Z->First);
		Z->First = ukazatel;									// posunuti fronty
	}
	return val;
}
//--------------------------//
//		LINEARNI SEZNAM		//
//--------------------------//
void InitList (tInstrList *L) {
	L->last = NULL; 	
	L->first = NULL;
}
void DisposeList (tInstrList *L) {
	tInstrListElem *ukazatel = NULL;	
	while(L->first != NULL) {
		ukazatel = L->first->next;
		free(L->first);
		L->first = ukazatel;
	}
	InitList(L);
}
void InsertLast (tInstrList *L, tInstruction *data) {
	tInstrListElem* ukazatel = NULL;
	if(L->last != NULL) ukazatel = L->last;
	tInstrListElem* struktura = malloc(sizeof(tInstrListElem));
	if(struktura != NULL) {
	 	L->last = struktura;
	 	L->last->data = data;
	 	L->last->next = NULL;
	 	ukazatel->next = L->last;
	}
	else Error();
}
tInstruction* GetFirst (tInstrList *L) {
	tInstrListElem* ukazatel = NULL;
	if (L->first == NULL) return NULL;
	if (L->first->next != NULL) ukazatel = L->first->next;
	if (L->first == L->last) L->last = NULL;
	tInstruction* data = NULL;
	data = L->fist->data;
	free(L->first);
	L->first = ukazatel;
	return data;
}	
//--------------------------//
//		GRAMATIKA			//
//--------------------------//
int PrecG(tExpr *suc1, tExpr *suc2, tExpr *suc3) {
	switch (suc1->terminal) {
		case NETERMINAL:
			if (suc3 == NULL)
				error;
			if (suc3->type == NETERMINAL) {
				switch (suc2->type) {
					case PLUS: 			return 1;
					case MINUS: 		return 2;
					case MULTIPLY: 		return 3;
					case DIVIDE: 		return 4;
					case GREATER: 		return 5;
					case GREATER_ROVNO: return 6;
					case LESS: 			return 7;
					case LESS_ROVNO: 	return 8;
					case EQUAL: 		return 9;
					case NEGACE: 		return 10;
				}
			}
		break;
		case TERMINAL:
			switch (suc1->type) {
				case STRING:
				case IDENTIFIER:
				case DOUBLE_NUMBER:
				case INT_NUMBER:
					return 11;
				case L_BRACKET:
					if (suc2 == NULL || suc3 == NULL)
						error;
					if (suc2->type == NETERMINAL)
						if (suc3->type == R_BRACKET)
							return 12
				break;
			}
	}
	return 0;
}
//------------------------------//
// 		  GENEROVANI 3AK 		//
//------------------------------//
void TrojAdres();

//------------------------------//
// 	 PREC. SEMANTICKA ANALYZA	//
//------------------------------//
tExpr* SemId(tExpr* kralik) {
	hashElem* element;
	element = findElem(tabulka, kralik.data);
	free(kralik.data)
	if (element == NULL)
		return 3; // nedefinovana prom
	kralik.data = &element->data.value;
	kralik.type = element->data.type;
	return kralik;
}

tExpr* SemA(tExpr *expr1, tExpr *expr2, tExpr *expr3, int gramatika) {

	if (expr1.type == IDENTIFIER)
		SemId(expr1);
	if (gramatika != 11) { // !!!!!!!!!!!!!!!!
	if (expr2.type == IDENTIFIER)
		SemId(expr2);
	if (expr3.type == IDENTIFIER)
		SemId(expr3);
	if ((expr1.type == STRING && (expr3.type == DOUBLE_NUMBER ||
		expr3.type == INT_NUMBER)) || ((expr1.type == DOUBLE_NUMBER ||
		expr1.type == INT_NUMBER) && expr3.type == STRING))
		return kua;
	}
	tExpr *struktura;
	if ((struktura = malloc(sizeof(tExpr))) == NULL)
		return kua; // chyba
	struktura->terminal = NETERMINAL;
	switch gramatika {
		case 1:
		case 2:
		case 3:
		case 4:
			if (expr1.type == DOUBLE_NUMBER || expr3.type == DOUBLE_NUMBER) {
				struktura->type = DOUBLE_NUMBER;
				if ((struktura->data = malloc(sizeof(double))) == NULL)
					return 5;
			}
			else {
				struktura->type = INT_NUMBER;
				if ((struktura->data = malloc(sizeof(int))) == NULL)
					return 5;
			}
			TrojAdres(gramatika, expr1.data, expr3.data, struktura.data);
			break;
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
			if (expr1.type == INT_NUMBER && expr3.type == DOUBLE_NUMBER) {
				int docasna_prom = *((int *)expr1.data);
				if ((expr1.data = realloc(expr1.data, sizeof(double))) == NULL)
					return chyba;
				*expr1.data = docasna_prom;
			}
			if (expr1.type == DOUBLE_NUMBER && expr3.type == INT_NUMBER) {
				int docasna_prom = *((int *)expr3.data);
				if ((expr3.data = realloc(expr3.data, sizeof(double))) == NULL)
					return chyba;
				*expr3.data = docasna_prom;
			}
			if ((expr1.type == INT_NUMBER && expr3.type == INT_NUMBER) ||
			(expr1.type == DOUBLE_NUMBER && expr3.type == DOUBLE_NUMBER) ||
			(expr1.type == STRING && expr3.type == STRING)) {
				struktura->type = INT_NUMBER;
				if ((struktura->data = malloc(sizeof(int))) == NULL)
					return 5;
				TrojAdres(gramatika, expr1.data, expr3.data, struktura.data);
			}
			else return 4;
		break;
		case 11:
			if (expr1.type == INT_NUMBER) struktura->type = INT_NUMBER;
			else if (expr1.type == DOUBLE_NUMBER) struktura->type = DOUBLE_NUMBER;
			else if (expr1.type == STRING) struktura->type = STRING;
			else chyba;
			TrojAdres(gramatika, expr1.data, NULL, struktura.data);
	}
	return struktura;
}
//------------------------------//
//	PRECEDENCNI SYNTAX ANALYZA	//
//------------------------------//
int PrecedencniSA () {
	int val, gramatika;				// promenne
	tExpr* input, suc1, suc2, suc3, top, haf;
	tList* Z;											// zasobnik
	Init(Z);
	if ((haf = malloc(sizeof(tExpr)))== NULL)
		return error;
	haf->type = SEMICOLON;
	haf->terminal = TERMINAL;
	Push(Z, haf);
	input = NextToken();									// input
	top = FirstTerminal(Z);								// nacteni pocatku zasobniku
	while (top->type != SEMICOLON && input->type != SEMICOLON) {	// dokud není vstup i zasobnik prazdny
		top = FirstTerminal(Z);							// nacti dalsi terminal
		switch (precedencni_tabulka[OperatorToIndex(top)][OperatorToIndex(input)])	// najdi pravidlo v prec. tabulce
			case '=': 									// pokud se rovnaji
				Push(Z, input);							// zapis do zasobniku
				input = NextToken();					// nacti dalsi znak na vstupu
				break;
			case '<': 									// pokud ma zasobnik mensi prioritu nez vstup (dle tabulky)
				tExpr* baf;
				if ((baf = malloc(sizeof(tExpr))) == NULL)
					return error;
				baf->type = 5948;
				Push(Z, baf);							// zapis do zasobniku znak <
				Push(Z, input);							// zapis vstup
				input = NextToken();					// nacti dalsi vstup
				break;
			case '>':									// pokud ma zasobnik vetsi prioritu nez vstup (dle tabulky)
				do {
					suc3 = suc2;						// zapis 3. predchudce
					suc2 = suc1;						// zapis 2. predchudce
					suc1 = top;							// zapis prvniho predchudce
					top = Pop(Z);						// vytahni prvek na top
				} while (top->type != 5948)					// dokud v promennych nemam cely retezec, pro ktery hledam pravidlo
				gramatika = PrecG(suc1, suc2, suc3);
				if (top->type == 5948 && gramatika != 0) { 
					free(top);
					top = SemA(suc1, suc2, suc3, gramatika);
					Push(Z, top);	// kontrola na nalezene pravidlo, pokud existuje, prepise na E -> neterminal
				}
				else error();
				free(suc1);
				free(suc2);
				free(suc3);
				suc3 = suc2 = suc1 = NULL;
				break;
			case ' ': 									// chyba ve vyrazu
				error();
				break;
	}
	Dispose(Z);											// uvolneni zasobniku

}

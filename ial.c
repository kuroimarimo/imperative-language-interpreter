#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>




int lenght(char *s)
{
int x=0;
int pocet=0;

while(s[x]!='\0'){
pocet++;
putchar(s[x]);
	if(s[x]== '\\'){
	pocet--;
		if((s[x+1]=='x')&&(isdigit(s[x+2]))){
			pocet=pocet-2;
		}
		if((s[x+1]=='x')&&(isdigit(s[x+3]))){
			pocet--;
		}

	}

x++;
}
pocet--;
return pocet;

}

int main(){
char test []={"znaků\n \x2"};

printf("%d",lenght(test));

}
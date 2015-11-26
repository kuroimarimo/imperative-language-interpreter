#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "ial.h"
#include "error.h"

/* hashes string 'str' for hash table of size 'hTabSize' */
unsigned int hFunct(const char *str, unsigned hTabSize)
{
    unsigned int h = 0;
    const unsigned char *p;
    for (p = (const unsigned char*)str; *p != '\0'; p++)
        h = 65599 * h + *p;
    return h % hTabSize;
}


bool checkOverfill (unsigned int size, unsigned int numStoredElem)
{
    return (numStoredElem >= 0.6 * size);
}

/* creates a new hash table of size 'size' */
hTab * hTabInit(unsigned int size)
{
    hTab * hashTable = malloc(sizeof(hTab));
    if (!hashTable)
        
        return NULL;            // malloc failed ERROR!
    
    if ((hashTable->table = malloc(size * sizeof(hashElem *))) == NULL)
    {
        free(hashTable);
        return NULL;            // malloc failed ERROR!
    }
    
    hashTable->size = size;
    hashTable->numStoredElem = 0;
    
    for (unsigned int i = 0; i < size; i++) hashTable->table[i] = NULL;
    
    return hashTable;
}

/* doubles the size  of the table */
hTab * expandTab(hTab * table)
{
    hashElem * tmp;
    
    unsigned int newSize = 2 * table->size;
    hTab * newTable = hTabInit(newSize);
    
    for (unsigned int i = 0; i < table->size; ++i)
    {
        if (table->table[i])
        {
            tmp = table->table[i];
            
            addElem(newTable, tmp->key, tmp->data);
            
            while (tmp->next)           // in case there are synonyms
            {
                tmp = tmp->next;
                addElem(newTable, tmp->key, tmp->data);
            }
        }
    }
    
    hTabFree(table);
    return newTable;
}

/* adds new element to a hash table */
hashElem * addElem (hTab * table, char * key, tData data)
{
    hashElem * newElem;
    
    //if (checkOverfill(table->size, table->numStoredElem))
    //{
    //    table = expandTab(table);
    //}
    
    newElem = table->table[hFunct (key, table->size)];
    
    while (newElem)             // while there's a collision
    {
        if (!strcmp(key, newElem->key)) // if there's already an element with such key, replace the data it holds
        {
            newElem->data = data;
            return newElem;
        }
        
        newElem = newElem->next;
    }
    
    // no element with such key
    
    if ((newElem = malloc(sizeof(hashElem))) == NULL)
        return NULL;        // malloc failed ERROR!
    
    newElem->next = table->table[hFunct(key, table->size)];
    
    newElem->key = malloc((strlen(key) + 1) * sizeof(char));
    strcpy(newElem->key, key);
    
    newElem->data = data;
    
    if (newElem->data.type == var_string)
    {
        newElem->data.value.string_value = malloc((strlen(data.value.string_value)+1) * sizeof(char));
        strcpy(newElem->data.value.string_value, data.value.string_value);
    }
    
    table->table[hFunct(key, table->size)] = newElem;
    
    ++table->numStoredElem;
    
    return newElem;
}


/* returns pointer to the element with key 'key' or NULL if there's no such element */
hashElem * findElem (hTab * table, char * key)
{
    hashElem * searchdFor = table->table[hFunct(key, table->size)];
    
    while ((searchdFor != NULL) && strcmp(key, searchdFor->key))
    {
        searchdFor = searchdFor->next;
    }
    
    return searchdFor;
}

/* removes element with key 'key' from the table */
void removeElem (hTab * table, char * key)
{
    hashElem * tmp = table->table[hFunct(key, table->size)];
    hashElem * removed;
    
    if (!strcmp(key, tmp->key))      // no synonyms pointing to the element that's being removed
    {
        table->table[hFunct(key, table->size)] = tmp->next;
        
        if (tmp->data.type == var_string)
            free(tmp->data.value.string_value);
        
        free(tmp);
        return;
    }
    
    else        // synonyms pointing to the element that's being removed -> we have to relink the list
    {
        while ((tmp->next != NULL) && strcmp(key, tmp->next->key))      // finds the preceding element to the element to be removed
        {
            tmp = tmp->next;
        }
        
        removed = tmp->next;
        tmp->next = removed->next;              // relinks the linked list, so the element 'removed' can be freed
        
        if (removed->data.type == var_string)
            free(removed->data.value.string_value);
        
        free(removed->key);
        free(removed);
    }
}


/* frees the whole table */
void hTabFree (hTab * table)
{
    hashElem * tmp = NULL;
    hashElem * elem;
    
    for (unsigned int i = 0; i < table->size; ++i)
    {
        elem = table->table[i];
        while (elem)
        {
            tmp = elem->next;
            
            if (elem->data.type == var_string)
                free(elem->data.value.string_value);
            
            free(elem->key);
            free(elem);
            elem = tmp;
        }
    }
    free(table->table);
    free(table);
}

int lenght(char *s)
{
    return strlen(s);
/*int x=0;
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
return pocet;*/
}

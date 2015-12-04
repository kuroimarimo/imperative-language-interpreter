#include "ial.h"

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


/* Autamatically allocates memory and copies string from 'src' to 'dest'.
 Return value depend on whether the allocation has succeeded or not. */
char * strDuplicate(char * src)                                // UNFINISHED!
{
    size_t srcLength;
	char * dest;
    //char * tmp = * dest;
    
    srcLength = strlen(src) + 1;                // + 1 takes into account the end of string character
    
	dest = customMalloc((int) srcLength * sizeof(char));
    
    if ((dest = strcpy(dest, src)) == NULL)
		return NULL;							//zavolat error.h
    
    return dest;
}


/* creates a new hash table of size 'size' */
hTab * hTabInit(unsigned int size)
{
    hTab * hashTable = customMalloc(sizeof(hTab));
    if (!hashTable)

        return NULL;            // customMalloc failed ERROR!

    if ((hashTable->table = customMalloc(size * sizeof(hashElem *))) == NULL)
    {
        //free(hashTable);
        return NULL;            // malloc failed ERROR!
    }

    hashTable->size = size;
    hashTable->numStoredElem = 0;

    for (unsigned int i = 0; i < size; i++) hashTable->table[i] = NULL;

    return hashTable;
}

void hashElemInit (hashElem * elem)
{
    elem->data.type = -1;
    elem->data.state = DECLARED;
    elem->data.numberOfParams = 0;
	elem->data.value.int_value = 0;
	elem->data.params = NULL;
    
    //if (elem->data.fParamTypes != NULL)
   /*     free(elem->data.fParamTypes);
    elem->data.fParamTypes = NULL;*/
    
    //if (elem->data.localTable != NULL)
    //    hTabFree(elem->data.localTable);
    //elem->data.localTable = NULL;
    
    //if (elem->key != NULL)
        //free(elem->key);
    elem->key = NULL;
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

            addElem(newTable, tmp->key, &tmp->data);

            while (tmp->next)           // in case there are synonyms
            {
                tmp = tmp->next;
                addElem(newTable, tmp->key, &tmp->data);
            }
        }
    }

    hTabFree(table);
    return newTable;
}

/* Deep-copies the data from 'srcData' to 'destData' given that both are of type tData.
    (Except params, 'cause reasons. (╯°□°）╯︵ ┻━┻ )
   Returns true upon successful completion, otherwise false.*/
bool tDataCopy (tData * destData, tData * srcData)
{
    destData->type = srcData->type;
    destData->state = srcData->state;
    destData->value = srcData->value;
	destData->numberOfParams = srcData->numberOfParams;
    
    //destData->localTable = srcData->localTable;   // not used if the element is variable; if it's a function, the table is later initialized
    
    /*if (srcData->fParamTypes != NULL)			// there's data to copy
    //    return (strDuplicate(&destData->fParamTypes, &srcData->fParamTypes));
    {
        destData->fParamTypes = malloc((strlen(srcData->fParamTypes)+1) * sizeof(char));
        strcpy(destData->fParamTypes, srcData->fParamTypes);

        return destData->fParamTypes;
    }

    else
    {
        destData->fParamTypes = NULL;
        return true;
    }*/
	destData->params = srcData->params;
	return true;
    
}

/* adds new element to a hash table */
hashElem * addElem (hTab * table, char * key, tData * data)
{
    hashElem * newElem;

    if (checkOverfill(table->size, table->numStoredElem))
    {
        table = expandTab(table);
    }

    newElem = table->table[hFunct (key, table->size)];

    while (newElem)             // while there's a collision
    {
        if (!strcmp(key, newElem->key)) // if there's already an element with such key, replace the data it holds
        {
            if (tDataCopy(&newElem->data, data))
            {
                return NULL;                    // Malloc failed.
            }
            return newElem;
        }

        newElem = newElem->next;
    }

    // no element with such key

    if ((newElem = customMalloc(sizeof(hashElem))) == NULL)
        return NULL;        // malloc failed ERROR!

    newElem->next = table->table[hFunct(key, table->size)];

    /*newElem->key = customMalloc(((int) strlen(key) + 1) * sizeof(char));
    strcpy(newElem->key, key);*/
	newElem->key = strDuplicate(key);

//    if (!strDuplicate(&newElem->key, &key))
//        return NULL;                    // Malloc failed.
    
    if (!tDataCopy(&newElem->data, data) || !newElem->key)
    {
        return NULL;                    // Malloc failed.
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

        /*if (tmp->data.type == VAR_STRING)
            free(tmp->data.value.string_value);*/

        /*if (tmp->data.fParamTypes != NULL)
            free(tmp->data.fParamTypes);

        if (tmp->data.localTable != NULL)
            hTabFree(tmp->data.localTable);*/
		
		/*if (tmp->data.params != NULL)
			free(tmp->data.params);

        free(tmp);*/
        return;
    }

    else        // synonyms pointing to the element that's being removed -> we have to relink the list
    {
        while (strcmp(key, tmp->next->key))         // finds the preceding element to the element to be removed
        {
            if (tmp->next != NULL)
                return;
            tmp = tmp->next;                        // end of list and no such element found
        }

        removed = tmp->next;
        tmp->next = removed->next;              // relinks the linked list, so the element 'removed' can be freed

        /*if (removed->data.type == VAR_STRING)
            free(removed->data.value.string_value);*/

        /*if (removed->data.fParamTypes != NULL)
            free(removed->data.fParamTypes);

        if (removed->data.localTable != NULL)
            hTabFree(removed->data.localTable);*/
		/*if (removed->data.params != NULL)
			free(tmp->data.params);

        free(removed->key);
        free(removed);*/
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

            /*if (elem->data.type == VAR_STRING)
                free(elem->data.value.string_value);
			elem->data.value.string_value = NULL;*/

           /* if (elem->data.fParamTypes != NULL)
                free(elem->data.fParamTypes);
			elem->data.fParamTypes = NULL;

            if (elem->data.localTable != NULL)
                hTabFree(elem->data.localTable);*/

			/*if (elem->data.params != NULL)
				free(elem->data.params);

            free(elem->key);
            free(elem);*/
            elem = tmp;
        }
    }
    /*free(table->table);
    free(table);*/
	table = NULL;
}

int lenght(char *s)
{
    return (int) strlen(s);

}

char *substr (char *s, int i, int n) {

    int len = (int) strlen(s);    /// length of string
    char * subs = NULL;
    subs = customMalloc((len + 1)*sizeof(char));

    if (subs == NULL) {
        /// chyba 10;
        return NULL;
    }

    strncpy(subs, s+i, n);

    return subs;
}


char *concat (char *s1, char *s2){

int lenght1=(int) strlen(s1);  //delka prvního
int lenght2=(int) strlen(s2);   //delka druhého

char *pomocna= customMalloc((lenght1 + lenght2)*sizeof(char));  // naalokuju si pamět velikosti prvniho a druheho řetězce
if(pomocna==NULL) return NULL ;
strncpy(pomocna,s1,lenght1);  //zkopiruju prvni řetězec do pomocne 
strncpy(pomocna+lenght1,s2,lenght2);  //zkopiruju druhy řetězec na pozici lenght1 -1  musime odstranit (\0)
return pomocna; 
}


int find(char*s, char*search){

int lenght_s=(int)strlen (s);
int lenght_search=(int) strlen (search);
if (lenght_search==0)  //pokud je podřetězec prázdný vrať nulu 
    return 0;
int counter_s=0;   //počítadlo projitých znaků řetězce s 
int counter_found_search=0;   //počitadlo nalezených stejných znaků 



        while (counter_s<lenght_s){  //dokud je počítadlo projitých znaků menší než délka 
            
            

            if (s[counter_s]!=search[counter_found_search]) {  //pokud jsou znaky rozdílné, počet stejných znaků dej na nulu
            counter_found_search=0;
            }
            
            if( s[counter_s]==search[counter_found_search]){  //pokud jsou znaky stejné 
                
                    counter_found_search++;   //inkrementuj počítadlo najítých znaků 
                                
                    if (counter_found_search==lenght_search) { //pokud je počet najitých znaků stejný jako délka substringu 
                        
                            return counter_s-counter_found_search+1;   //vrať rozdíl +1 - pozice počítaná od nuly 
                    }
                    
            }
                    

            counter_s++;    

        }


return -1; //pokud jsem nenašel vratím -1

    
}



char *sort (char *s) {
    int left = 0;
    int right = (int) (strlen(s)) - 1;

    sort_recursive(s, left, right);

    return s;
}

char *sort_recursive(char *s, int left_poc, int right_poc) {

    int left = left_poc;
    int right = right_poc;
    int middle = s[(left_poc + right_poc) / 2]; /// pseudomedián

    do {
        while (s[left] < middle && left < right_poc)
            left++;
        while (s[right] > middle && right > left_poc)
            right--;

        if (left <= right)  /// výměna prvků
        {
            int pom = s[left];
            s[left] = s[right];
            s[right] = pom;

            left++;
            right--;
        }

    } while (left < right);

    if (right > left_poc)
        sort_recursive(s, left_poc, right);
    if (left < right_poc)
        sort_recursive(s, left, right_poc);

    return s;
}

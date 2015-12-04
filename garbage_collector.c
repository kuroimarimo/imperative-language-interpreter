#include "garbage_collector.h"

void garbageListInit(tGarbageList * list)
{
	list->first = list->last = NULL;
}

void * appendGarbage(tGarbageList * list, void * data)
{
	struct garbage * temp = malloc(sizeof(struct garbage));
	if (temp == NULL)
		return NULL;

	temp->next = NULL;
	temp->data = data;

	if (list->first == NULL)				//the list is empty
		list->first = temp;
	else
		list->last->next = temp;

	list->last = temp;
	return temp;
}

void  updateGarbageItem(tGarbageList * list, void * old, void * new)
{
	struct garbage * temp = list->first;

	if (temp == NULL)
	{
		if (appendGarbage(list, new) == NULL)
			fatalError(ERR_AllocFailed);
		return;
	}

	while (temp != NULL)
	{
		if (temp->data == old)
		{
			temp->data = new;
			return;
		}

	temp = temp->next;
	}
}

void * customMalloc(int size)
{
	void * temp = malloc(size);
	if (temp == NULL)
		return NULL;			//volanie error.h funkcie

	if (appendGarbage(&garbageList, temp) == NULL)
	{
		free(temp);				//clean malloc-ed data that hasn't been saved to garbageList
		fatalError(ERR_AllocFailed);
	}
	
	return temp;
}

void * customRealloc(void * data, int size)
{

	if (data == NULL)
		return customMalloc(size); 

	void * temp = realloc(data, size);
	
	if (temp == NULL)
		fatalError(ERR_AllocFailed);

	if (temp != data)
		updateGarbageItem(&garbageList, data, temp);
	
	return temp;
}

void garbageCleanup(tGarbageList * list)
{
	struct garbage * temp = list->first;
	void * next;

	while (temp != NULL)
	{
		if (temp->data != NULL)
			free(temp->data);

		next = temp->next;
		free(temp);
		temp = next;
	}

	list->first = list->last = NULL;
}

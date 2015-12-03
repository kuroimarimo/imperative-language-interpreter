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

void   deleteGarbageItem(tGarbageList * list, void * deleted)
{
	struct garbage * temp = list->first;

	if (temp == NULL)
		return;

	if (temp->data == deleted)			//the first item is being deleted
	{
		list->first = temp->next;
		
		if (temp->next == NULL)			//the first item is also the last item
			list->last = NULL;
		
		free(temp);
		return;
	}

	while (temp->next != NULL)
	{
		if (temp->next->data == deleted)
		{
			if (temp->next->next == NULL)	//we are removing the last item
			{
				free(temp->next);
				temp->next = NULL;
				list->last = temp;
				return;
			}
			else
			{
				void * next = temp->next->next;
				free(temp->next);
				temp->next = next;			//relink the list
				return;
			}
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
		return NULL;			//error.h
	}
	else
		return temp;
}

void * customRealloc(void * data, int size)
{

	if (data == NULL)
		return customMalloc(size); 

	void * temp = realloc(data, size);
	if (temp == NULL)
		return NULL;			//error.h

	if (temp != data)
		deleteGarbageItem(&garbageList, data);

	if (appendGarbage(&garbageList, temp) == NULL)
	{
		free(temp);
		return NULL;					//error.h
	}
	else
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
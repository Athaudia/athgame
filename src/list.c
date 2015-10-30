#include "list.h"
#include <stdlib.h>

struct ag_list_elem
{
	void* data;
	struct ag_list_elem* previous;
	struct ag_list_elem* next;
};

struct ag_list_iter
{
	struct ag_list_elem* elem;
	struct ag_list* list;
};

struct ag_list_elem* ag_list_elem__new(void* data, struct ag_list_elem* previous, struct ag_list_elem* next)
{
	struct ag_list_elem* elem = (struct ag_list_elem*)malloc(sizeof(struct ag_list_elem));
	elem->data = data;
	elem->previous = previous;
	elem->next = next;
	return elem;
}

void ag_list_elem__destroy(struct ag_list_elem* elem)
{
	free(elem);
}

//----------ag_list

struct ag_list* ag_list__new()
{
	struct ag_list* list = (struct ag_list*)malloc(sizeof(struct ag_list));
	list->back = 0;
	list->front = 0;
	return list;
}

void ag_list__destroy(struct ag_list* list)
{
	struct ag_list_elem* elem = list->front;
	while(elem)
	{
		struct ag_list_elem* next = elem->next;
		ag_list_elem__destroy(elem);
		elem = next;
	}
	free(list);
}

void ag_list__push_front(struct ag_list* list, void* data)
{
	if(list->front == 0)
		list->back = list->front = ag_list_elem__new(data, 0, 0);
	else
	{
		list->front = ag_list_elem__new(data, 0, list->front);
		list->front->next->previous = list->front;
	}
}

void ag_list__push_back(struct ag_list* list, void* data)
{
	if(list->back == 0)
		list->back = list->front = ag_list_elem__new(data, 0, 0);
	else
	{
		list->back = ag_list_elem__new(data, list->back, 0);
		list->back->previous->next = list->back;
	}
}

struct ag_list_iter* ag_list_iter__new(struct ag_list* list)
{
	struct ag_list_iter* iter = (struct ag_list_iter*)malloc(sizeof(struct ag_list_iter));
	iter->elem = 0;
	iter->list = list;
	return iter;
}

void ag_list_iter__destroy(struct ag_list_iter* iter)
{
	free(iter);
}

bool ag_list_iter__next(struct ag_list_iter* iter)
{
	if(iter->elem)
		iter->elem = iter->elem->next;
	else
		iter->elem = iter->list->front;
	return iter->elem;
}

bool ag_list_iter__previous(struct ag_list_iter* iter)
{
	iter->elem = iter->elem->previous;
	return iter->elem;
}

void* ag_list_iter__get(struct ag_list_iter* iter)
{
	return iter->elem->data;
}

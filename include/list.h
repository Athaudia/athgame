#pragma once

#include <stdbool.h>

struct ag_list_elem;
struct ag_list_iter;

struct ag_list
{
	struct ag_list_elem* front;
	struct ag_list_elem* back;
};

struct ag_list* ag_list__new();
void ag_list__destroy(struct ag_list* list);
void ag_list__push_front(struct ag_list* list, void* data);
void ag_list__push_back(struct ag_list* list, void* data);

struct ag_list_iter* ag_list_iter__new(struct ag_list* list);
void ag_list_iter__destroy(struct ag_list_iter* iter);
bool ag_list_iter__next(struct ag_list_iter* iter);
bool ag_list_iter__previous(struct ag_list_iter* iter);
void* ag_list_iter__get(struct ag_list_iter* iter);

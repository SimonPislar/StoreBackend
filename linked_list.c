#include "linked_list.h"
#include <stdlib.h>
#include <CUnit/Basic.h>
#include <stdbool.h>
#include <stddef.h>
#include "common.h"

struct list 
{
    struct link *first;
    struct link *last;
    ioopm_eq_function eq_fn;
};


struct link 
{
    elem_t element;
    link_t *next;
};

static link_t *link_create(elem_t element, link_t *next)
{
    link_t *link = (link_t*) calloc(1, sizeof(link_t));
    link->element = element;
    link->next = next;
    return link;
}

ioopm_list_t *ioopm_linked_list_create(ioopm_eq_function eq_func)
{
    ioopm_list_t *list = calloc(1, sizeof(ioopm_list_t));
    list->eq_fn = eq_func;
    return list;
}

static void destroy_links(link_t *first) 
{
    if (first->next != NULL) {
        link_t *pn = first->next;
        destroy_links(pn);
        }
    free(first);
}


void ioopm_linked_list_clear(ioopm_list_t *list)
{
    if (list->first != NULL) {
    destroy_links(list->first);
    }
    list->first = NULL;
    list->last = NULL;
}

void ioopm_linked_list_destroy(ioopm_list_t *list)
{
    ioopm_linked_list_clear(list);
    free(list);
}

void ioopm_linked_list_append(ioopm_list_t *list, elem_t element)
{
    if (list->first == NULL) {
        list->first = link_create(element, NULL);
        list->last = list->first;
    }
    else {
        list->last->next = link_create(element, NULL);
        list->last = list->last->next;
    }
}

void ioopm_linked_list_prepend(ioopm_list_t *list, elem_t element)
{
    if (list->first == NULL) {
        list->first = link_create(element, NULL);
        list->last = list->first;
    }
    else {
        link_t *l = list->first;
        list->first = link_create(element, l);
    }
}

bool ioopm_linked_list_is_empty(ioopm_list_t *list)
{
    if (list->last == NULL && list->first == NULL) {
        return true;
    }
    return false;
}

size_t ioopm_linked_list_size(ioopm_list_t *list)
{
    int counter = 0;
    link_t *current = list->first;
    while (current != NULL) {
        counter++;
        current = current->next;
    }
    return counter;
}

option_t ioopm_linked_list_get(ioopm_list_t *list, int index)
{
    link_t *current = list->first;
    size_t size = ioopm_linked_list_size(list);
    if (size > 0 && size - 1 >= index && index >= 0 && current != NULL) {
        for(int i = 0; i != index; i++) {
            current = current->next;
            }
        return Success(current->element);
        }
    else return Failure();
}



void ioopm_linked_list_insert(ioopm_list_t *list, int index, elem_t element)
{
    if (ioopm_linked_list_size(list) == index) {
        ioopm_linked_list_append(list, element);
        }
    else if (index == 0) { // added else so it we dont insert twice if index == 0 == size
        ioopm_linked_list_prepend(list, element);
    }
    else if (ioopm_linked_list_size(list) >= index && index >= 0) {
        link_t *current = list->first;
        link_t *previous = current;
        for(int i = 0; i != index; i++) {
            previous = current;
            current = current->next;
            }
        previous->next = link_create(element, previous->next);
        }
}

bool ioopm_linked_list_contains(ioopm_list_t *list, elem_t element)
{
    if (ioopm_linked_list_size(list) > 0) {
        link_t *current = list->first;
        while (current != NULL) {
            if (list->eq_fn(current->element, element)) {
                return true;
                }
            current = current->next;
            }
        }
    return false;
}

option_t ioopm_linked_list_remove(ioopm_list_t *list, size_t index)
{   
    elem_t removed_element = ioopm_linked_list_get(list, index).element;
    size_t size = ioopm_linked_list_size(list);
    if (size > 0 && size > index && index >= 0) { // changed from size >= index to size > index
        link_t *to_remove = list->first;
        link_t *previous = to_remove;
        if (index == 0) {
            list->first = to_remove->next;
            if(index == size-1){list->last = NULL;} // changing last to NULl if you remove the only element in a list
        }
        else {
            for(int i = 0; i != index; i++) {
                previous = to_remove;
                to_remove = to_remove->next;
            }
            previous->next = to_remove->next;
            if(index == size-1) {list->last = previous;} // set last to previous if index = size-1
        }
        free(to_remove);
        return Success(removed_element);
    }
    else return Failure();
}


bool ioopm_linked_list_all(ioopm_list_t *list, ioopm_predicate prop, void *extra)
{
    link_t *current = list->first;
    int counter = 0;
    while (current != NULL) {
        if (!prop(int_elem(counter), current->element, NULL, NULL, extra)) {
            return false;
        }
        current = current->next;
        counter++;
    }
    return true;
}

bool ioopm_linked_list_any(ioopm_list_t *list, ioopm_predicate prop, void *extra)
{
    link_t *current = list->first;
    int counter = 0;
    while (current != NULL) {
        if (prop(int_elem(counter), current->element, NULL, NULL, extra)) {
            return true;
        }
        current = current->next;
        counter++;
    }
    return false;
}

void ioopm_linked_list_apply_to_all(ioopm_list_t *list, ioopm_apply_function fun, void *extra)
{
    link_t *current = list->first;
    int counter = 0;
    while (current != NULL) {
        fun(int_elem(counter), &current->element, extra);
        current = current->next;
        counter++;
    }
}

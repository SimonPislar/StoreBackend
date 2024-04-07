#include "linked_list.h"
#include "iterator.h"
#include <stdlib.h>
#include <CUnit/Basic.h>
#include <stdbool.h>

struct list 
{
    struct link *first;
    struct link *last;
};


struct link 
{
    elem_t element;
    link_t *next;
};


struct iter 
{
  link_t *current;
  ioopm_list_t *list;
};


ioopm_list_iterator_t *ioopm_list_iterator(ioopm_list_t *list)
{
    ioopm_list_iterator_t *iterator = calloc(1, sizeof(ioopm_list_iterator_t));
    iterator->current = list->first;
    iterator->list = list;
    return iterator;
}

bool ioopm_iterator_has_next(ioopm_list_iterator_t *iter)
{
    return iter->current != NULL && iter->current->next != NULL;
}


option_t ioopm_iterator_next(ioopm_list_iterator_t *iter)
{
    if (ioopm_iterator_has_next(iter)) {
        iter->current = iter->current->next;
        return Success(iter->current->element);
    }
    return Failure();
    
}

option_t ioopm_iterator_current(ioopm_list_iterator_t *iter)
{
    if (iter->current != NULL) {
    return Success(iter->current->element);
        }   
    return Failure();
}

void ioopm_iterator_destroy(ioopm_list_iterator_t *iter)
{ 
    free(iter);
}

void ioopm_iterator_reset(ioopm_list_iterator_t *iter)
{
    iter->current = iter->list->first;
}
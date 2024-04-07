#pragma once
#include "linked_list.h"
#include <stdbool.h>

typedef struct iter ioopm_list_iterator_t;

typedef struct option option_t;

typedef struct list ioopm_list_t;

/// @brief Create an iterator for a given list
/// @param list the list to be iterated over
/// @return an iteration positioned at the start of list
ioopm_list_iterator_t *ioopm_list_iterator(ioopm_list_t *list);

/// @brief Checks if there are more elements to iterate over
/// @param iter the iterator
/// @return true if there is at least one more element 
bool ioopm_iterator_has_next(ioopm_list_iterator_t *iter);



/** @brief Step the iterator forward one step
* @param iter the iterator
* @return an option containing a bool corresponding to the
* Success of the operation and the next element if successful. 

* @note when a function call results in a failure the iterator is not moved
* forward meaning its possible to keep calling the function even after
* it results in a failure one or more times. Important to keep track of
* if using the function as a way to step forward in an iterator.
*/
option_t ioopm_iterator_next(ioopm_list_iterator_t *iter);

/// @brief Reposition the iterator at the start of the underlying list
/// @param iter the iterator
void ioopm_iterator_reset(ioopm_list_iterator_t *iter);

/// @brief Return the current element from the underlying list
/// @param iter the iterator
/// @return an option containing bool corresponding to success of the operation
/// and the current element if operation was succesful.
option_t ioopm_iterator_current(ioopm_list_iterator_t *iter);

/// @brief Destroy the iterator and return its resources
/// @param iter the iterator
void ioopm_iterator_destroy(ioopm_list_iterator_t *iter);
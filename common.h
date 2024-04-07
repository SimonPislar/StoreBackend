#pragma once
#include <stddef.h>
#include <stdbool.h>

#define load_factor 10
#define Init_Buckets 17
#define Init_size_index 0
#define final_index 11

#define Success(v)      (option_t) { .success = true, .element = v };
#define Failure()       (option_t) { .success = false };
#define Successful(o)   (o.success == true)
#define Unsuccessful(o) (o.success == false)

#define int_elem(x) (elem_t) { .integer=(x) }
#define ptr_elem(x) (elem_t) { .pointer=(x) }

#define ioopm_int_str_ht_insert(ht, i, s) \
    ioopm_hash_table_insert(ht, int_elem(i), ptr_elem(s))

#define ioopm_int_str_ht_lookup(ht, i) \
    ioopm_hash_table_lookup(ht, int_elem(i))

#define ioopm_int_ht_remove(ht, i) \
    ioopm_hash_table_remove(ht, int_elem(i))

#define ioopm_linked_list_int_append(list, i) \
    ioopm_linked_list_append(list, int_elem(i))

#define ioopm_linked_list_int_prepend(list, i) \
    ioopm_linked_list_prepend(list, int_elem(i))

#define ioopm_linked_list_int_insert(list, i, v) \
    ioopm_linked_list_insert(list, i, int_elem(v))

#define ioopm_linked_list_int_contains(list, i) \
    ioopm_linked_list_contains(list, int_elem(i))

//An element
typedef union elem elem_t;

union elem
{
    int integer;
    size_t unsigned_int;
    bool boolean;
    float floating;
    void *pointer;
};



struct option
{
    bool success;
    elem_t element;
};

typedef int(*ioopm_hash_function)(elem_t a);

typedef bool(*ioopm_compare_func)(elem_t a, elem_t b);

typedef bool(*ioopm_predicate)(elem_t key, elem_t value, ioopm_compare_func comp_value_func, ioopm_compare_func comp_key_func, void *extra);

typedef void(*ioopm_apply_function)(elem_t key, elem_t *value, void *extra);

/// Compares two elements and returns true if they are equal
typedef bool(*ioopm_eq_function)(elem_t a, elem_t b);

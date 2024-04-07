#include "hash_table.h"
#include "common.h"
#include "linked_list.h"
#include "iterator.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include  <string.h>
#include <CUnit/Basic.h>
#include <math.h>
#include <time.h>

// Sizes for dynamic buckets
size_t primes[11] = {17, 31, 67, 127, 257, 509, 1021, 2053, 4099, 8191, 16381};

struct entry
{
    elem_t key;       // holds the key
    elem_t value;   // holds the value
    entry_t *next; // points to the next entry (possibly NULL)
};

struct hash_table
{
    size_t size_index;
    size_t *bucket_sizes;
    entry_t **buckets;
    ioopm_hash_function hash_func;
    ioopm_compare_func value_eq_fn;
    ioopm_compare_func key_eq_fn;
};


ioopm_hash_table_t *ioopm_hash_table_create(ioopm_hash_function hash_func, ioopm_compare_func value_eq_fn, ioopm_compare_func key_eq_fn){
    /// Allocate space for a ioopm_hash_table_t = 17 pointers to
    /// entry_t's, which will be set to NULL
    ioopm_hash_table_t *result = calloc(1, sizeof(ioopm_hash_table_t));
    result->bucket_sizes = primes;
    result->size_index = Init_size_index;
    result->buckets = calloc((result->bucket_sizes[result->size_index]), sizeof(entry_t));
    result->hash_func = hash_func;
    result->value_eq_fn = value_eq_fn;
    result->key_eq_fn = key_eq_fn;
    return result;
}


void static grow_insertion(ioopm_hash_table_t *ht, ioopm_list_t *keys, ioopm_list_t *values)
{
    ioopm_list_iterator_t *value_iter = ioopm_list_iterator(values);
    ioopm_list_iterator_t *key_iter = ioopm_list_iterator(keys);
    size_t size = ioopm_linked_list_size(keys);

    for(int i = 0; i < size; ++i) {
            ioopm_hash_table_insert(ht, ioopm_iterator_current(key_iter).element,  ioopm_iterator_current(value_iter).element);
            ioopm_iterator_next(key_iter);
            ioopm_iterator_next(value_iter); 
        }

    ioopm_iterator_destroy(value_iter);
    ioopm_iterator_destroy(key_iter);
    ioopm_linked_list_destroy(values);
    ioopm_linked_list_destroy(keys);
}

void static grow_prepare_for_insert(ioopm_hash_table_t *ht)
{
    ioopm_hash_table_clear(ht);
    
    ht->size_index++;
    
    free(ht->buckets);
    ht->buckets = calloc((ht->bucket_sizes[ht->size_index]), sizeof(entry_t));
}

 void static hash_table_grow(ioopm_hash_table_t *ht)
 {
    ioopm_list_t *value_list = ioopm_hash_table_values(ht);
    ioopm_list_t *key_list = ioopm_hash_table_keys(ht);
   
    grow_prepare_for_insert(ht);

    grow_insertion(ht, key_list, value_list);
 }

void static check_load_reached(ioopm_hash_table_t *ht){
    if (((double) (ioopm_hash_table_size(ht)+1) / (ht->bucket_sizes[ht->size_index])) >= load_factor){
        hash_table_grow(ht);
    }
} 


static void destroy_bucket(entry_t *p)
{   
     if (p->next != NULL) {
        entry_t *pn = p->next;
        destroy_bucket(pn);
        }
    free(p);
}


void ioopm_hash_table_clear(ioopm_hash_table_t *ht)
{
    for (int i = 0; i < (ht->bucket_sizes[ht->size_index]); i++)
    {
        entry_t *first = (ht->buckets[i]);
        if (first != NULL)
        {
            destroy_bucket(first);
        }
        ht->buckets[i] = NULL;
    }
}

void ioopm_hash_table_destroy(ioopm_hash_table_t *ht){
    ioopm_hash_table_clear(ht);
    free(ht->buckets);
    free(ht);
}


static entry_t **find_pointer_for_key(entry_t **bucket, elem_t key, ioopm_compare_func key_eq_func) 
{   
    if (*bucket && !(key_eq_func((*bucket)->key, key))) {
        return find_pointer_for_key(&(*bucket)->next, key, key_eq_func);
                }     
    else return bucket;
}


static entry_t *entry_create(elem_t key, elem_t value, entry_t *first_entry) 
{
    entry_t *new_entry = calloc(1, sizeof(entry_t));
    new_entry->key = key;
    new_entry->value = value;
    new_entry->next = first_entry;
    return new_entry;
}



void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value) 
{
    if (ht->size_index < final_index) {
        check_load_reached(ht);
    }
    
    size_t bucket_index = ht->hash_func(key) % (ht->bucket_sizes[ht->size_index]);
    entry_t **insert_point = find_pointer_for_key(&ht->buckets[bucket_index], key, ht->key_eq_fn);


    /// Check if the next entry should be updated or not
    if ((*insert_point) && (ht->key_eq_fn(((*insert_point)->key), key)))
        {
            (*insert_point)->value = value;
        }
    else
        {
            entry_t *new_entry = entry_create(key, value, NULL);
            new_entry->next = *insert_point;
            *insert_point = new_entry;
        }
}

option_t ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key){

    size_t bucket = ht->hash_func(key) % (ht->bucket_sizes[ht->size_index]);
    entry_t **wanted_entry = find_pointer_for_key(&ht->buckets[bucket], key, ht->key_eq_fn);

    if ((*wanted_entry) && (ht->key_eq_fn(((*wanted_entry)->key), key)))
    {
        return Success((*wanted_entry)->value);
    }
    else
    {
        return Failure();
    }
}


option_t ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key)
{
    size_t bucket = ht->hash_func(key) % (ht->bucket_sizes[ht->size_index]);
    elem_t value = ioopm_hash_table_lookup(ht, key).element;
    if (ioopm_hash_table_lookup(ht, key).success){
        entry_t **wanted_entry = find_pointer_for_key(&ht->buckets[bucket], key, ht->key_eq_fn);
        entry_t *to_remove = *wanted_entry;
        
        *wanted_entry = (*wanted_entry)->next;
        free(to_remove);

        return Success(value);
      
    } else return Failure();
}

// changed to size_t
static size_t bucket_size(entry_t *p, int counter) 
{
    while (p != NULL) {
        counter++;
        p = p->next;
    }
    return counter;
}

// changed to size_t
 size_t ioopm_hash_table_size(ioopm_hash_table_t *ht)
 {
  size_t counter = 0;
  for(int i = 0; i < (ht->bucket_sizes[ht->size_index]); i++) {
        counter = bucket_size(ht->buckets[i], counter);
    }
  return counter;
 }

bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht){
    for (int i = 0; i < (ht->bucket_sizes[ht->size_index]); ++i)
    {
        entry_t *first = (ht->buckets[i]);
        if(first != NULL)
        {
            return false;
        }
    }
    return true;
}

static void append_keys_bucket(entry_t *p, ioopm_list_t *key_list)
{
  while (p != NULL)
      {
        ioopm_linked_list_append(key_list, (p->key)); 
        p = p->next;
      }
}

ioopm_list_t *ioopm_hash_table_keys(ioopm_hash_table_t *ht)
{
  ioopm_list_t *key_list = ioopm_linked_list_create(NULL);
  for(int i = 0; i < (ht->bucket_sizes[ht->size_index]); ++i) {
    append_keys_bucket(ht->buckets[i], key_list);
  }
  return key_list;
}

// changed to size_t
static void append_values_bucket(entry_t *p, ioopm_list_t *values_list)
{
  while (p != NULL)
      {
        ioopm_linked_list_append(values_list, p->value);
        p = p->next;
      }

}

//changed to size_t
ioopm_list_t *ioopm_hash_table_values(ioopm_hash_table_t *ht)
{
  ioopm_list_t *values_list = ioopm_linked_list_create(NULL);
  for(int i = 0; i < (ht->bucket_sizes[ht->size_index]); ++i) {
    append_values_bucket(ht->buckets[i], values_list);
  }
  return values_list;
}


bool ioopm_hash_table_all(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg)
{
    size_t size = ioopm_hash_table_size(ht);
    ioopm_list_t *keys = ioopm_hash_table_keys(ht);
    ioopm_list_iterator_t *keys_iter = ioopm_list_iterator(keys);
    ioopm_list_t *values = ioopm_hash_table_values(ht);
    ioopm_list_iterator_t *values_iter = ioopm_list_iterator(values);
    bool result = true;
    for (int i = 0; i < size && result; ++i)
    {
        result = result && pred(ioopm_iterator_current(keys_iter).element, ioopm_iterator_current(values_iter).element, ht->value_eq_fn, ht->key_eq_fn, arg);
        ioopm_iterator_next(keys_iter);
    }
    ioopm_iterator_destroy(keys_iter);
    ioopm_iterator_destroy(values_iter);
    ioopm_linked_list_destroy(keys);
    ioopm_linked_list_destroy(values);
    return result;

}

bool ioopm_hash_table_any(ioopm_hash_table_t *ht, ioopm_predicate pred, void *arg)
{
    size_t size = ioopm_hash_table_size(ht);
    ioopm_list_t *keys = ioopm_hash_table_keys(ht);
    ioopm_list_iterator_t *keys_iter = ioopm_list_iterator(keys);
    ioopm_list_t *values = ioopm_hash_table_values(ht);
     ioopm_list_iterator_t *values_iter = ioopm_list_iterator(values);
    bool result = false;
    for (int i = 0; i < size; ++i)
    {
        result = pred(ioopm_iterator_current(keys_iter).element, ioopm_iterator_current(values_iter).element, ht->value_eq_fn, ht->key_eq_fn, arg);
        ioopm_iterator_next(keys_iter);
        if (result) 
        {
            ioopm_iterator_destroy(keys_iter);
            ioopm_iterator_destroy(values_iter);
            ioopm_linked_list_destroy(keys);
            ioopm_linked_list_destroy(values);
            return result;
        }
    }
    ioopm_iterator_destroy(keys_iter);
    ioopm_iterator_destroy(values_iter);
    ioopm_linked_list_destroy(keys);
    ioopm_linked_list_destroy(values);
    return result;

}

static bool key_equiv(elem_t key, elem_t value_ignored, ioopm_compare_func fn_value_eq_ignored, ioopm_compare_func fn_key_eq, void *x)
{
  elem_t *other_key_ptr = x;
  elem_t other_key = *other_key_ptr;
  return fn_key_eq(key, other_key);
}

bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key)
{
  return ioopm_hash_table_any(ht, key_equiv, &key);
}

static bool value_equiv(elem_t key_ignored, elem_t value, ioopm_compare_func fn_value_eq, ioopm_compare_func fn_key_eq_ignored, void *x)
{
    elem_t *other_value_ptr = x;
    elem_t other_value = *other_value_ptr;
    return fn_value_eq(value, other_value);
}

bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, elem_t value)
{ 
    return ioopm_hash_table_any(ht, value_equiv, &value);
}

void ioopm_hash_table_apply_to_all(ioopm_hash_table_t *ht, ioopm_apply_function apply_fun, void *arg)
{
    for (int i = 0; i < (ht->bucket_sizes[ht->size_index]); ++i)
    {
        entry_t *first = (ht->buckets[i]);
        if (first != NULL)
        {
            if (first->next == NULL)
            {
                apply_fun(first->key, &first->value, arg);
            }
            else {
                do
                {
                    apply_fun(first->key, &first->value, arg);
                    first = first->next;
                }  while (first->next != NULL);
                apply_fun(first->key, &first->value, arg);
            }
        }
    }
}
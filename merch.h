#pragma once
#include "hash_table.h"
#include "utils.h"
#include "db.h"

typedef struct item item_t;

typedef struct ioopm_db ioopm_db_t;

typedef struct item_info item_info_t;

/** @brief Allocates memory for and creates an item.
* @param name Name of item.
* @param desc description of item.
* @param price price of item.
* @return returnes created item.
*/
item_t *make_item(char * name, char *desc, int price);

/** @brief frees input item
* @param item item to destroy
*/
void ioopm_item_destroy(item_t *item);


/** @brief creates an item from user input.
* @return Created item.
*/
item_t *input_item();


/** @brief prints an item to the terminal.
* @param item item to be printed.
*/
void print_item(item_t *item);


/** @brief lists every item in the database
 * @param db database to be listed
 */
void ioopm_list_merchandise(ioopm_db_t *db);

/**
 * @brief shows stock count of a single item in the database
 * @param db the database where the stock is counted
 */
void ioopm_show_stock(ioopm_db_t *db);
bool ioopm_location_info_equiv_fn (elem_t a, elem_t b);
void print_list_of_merchandise(size_t length_of_merch, char** merch_arr);
option_t get_db_item_array(ioopm_db_t *db);
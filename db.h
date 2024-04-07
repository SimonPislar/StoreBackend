#pragma once
#include "utils.h"
#include "hash_table.h"
#include "merch.h"
#include "ui.h"

typedef struct ioopm_db ioopm_db_t;

typedef struct item item_t;

typedef struct location_info location_info_t;

typedef struct cart_item cart_item_t;

/** @brief creates a database
* @return created database
*/
ioopm_db_t *ioopm_db_create();

/** @brief Frees memory used by database
* @param db database to destroy
*/
void ioopm_db_destroy(ioopm_db_t *db);

/** @brief Inserts an item into a database
 * @param db database used to insert into
 */
void ioopm_db_insert(ioopm_db_t *db);

/**
 * @brief adds a new shopping cart to database
 * @param db database where cart is added
 */
void ioopm_db_add_cart(ioopm_db_t *db);

/** @brief removes an item from a database
 * @param db database to remove item from
 */
void ioopm_db_remove(ioopm_db_t *db);

/** @brief Edits item inside of database
 * @param db database where item is changed
 */
void ioopm_db_edit_item(ioopm_db_t *db);

/**
 * @brief allows user to make changes to item inside db
 * @param db where edits take place
 */
void ioopm_db_edit_item(ioopm_db_t *db);

/**
 * @brief increases stock of item on a single shelf
 * @param db database where we increase stock
 */
 void ioopm_db_replenish(ioopm_db_t *db);

/**
 * @param db
 * @param name_of_merch the name of the merch whose stock are going to count
 * @return the stock count on all locations combined
 */
int count_stock(ioopm_db_t *db, char *name_of_merch);

/**
 * @brief edits the name of an item inside of a database
 * @param db the database where the item exists
 * @param item_to_change the item whose name is to be changed
 */
void edit_item_name(ioopm_db_t *db, item_info_t *item_to_change);

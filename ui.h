#pragma once
#include "utils.h"
#include "db.h"
#include "merch.h"

typedef struct ioopm_db ioopm_db_t;

typedef struct item_info item_info_t;

/**
 * @brief displays a menu where users can take actions to change an item
 * @param db database where the item exists
 * @param item_to_change the item that is to be changed
 */
void edit_item_menu(ioopm_db_t *db, item_info_t *item_to_change);

/**
 * @brief Displays 20 merch at a time and provides user with the option to print more
 * @param length_of_merch the amount of different merch
 * @param merch_arr the array containing the merch
 */
void print_list_of_merchandise(size_t length_of_merch, char** merch_arr); 
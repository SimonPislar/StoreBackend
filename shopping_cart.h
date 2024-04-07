#include "common.h"
#include "linked_list.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct shopping_cart shopping_cart_t;

typedef struct cart_item cart_item_t;

typedef struct ioopm_db ioopm_db_t;

/**
 * @brief Create a cart list object
 * @return List that holds shopping carts for a database
 */
ioopm_list_t *create_cart_list();

/**
 * @brief Destroys a cart list
 * @param cart_list cart list to destroy
 */
void cart_list_destroy(ioopm_list_t *cart_list);

/**
 * @brief Create a shopping cart object
 * @param identifier The int that identifies the shopping cart
 * @return a shopping cart.
 */
shopping_cart_t *create_shopping_cart(int identifier);

/**
 * @brief destroys a shopping cart
 * @param cart shopping cart to destroyed
 */
void shopping_cart_destroy(shopping_cart_t *cart);

/**
 * @brief removes a shopping cart from a database
 * @param db database where cart is removed from
 * @return returns an option_t containing the success of the removal and 
 * if successful the identifier of the removed cart
 */
option_t ioopm_remove_shopping_cart(ioopm_db_t *db);

/**
 * @brief Removes a quantity of items from a shopping cart
 * @param db where shopping cart to remove from exists
 * @return returns an option_t containing the success of the removal and 
 * if successful the name of the removed item
 */
option_t ioopm_remove_from_cart(ioopm_db_t *db);


/**
 * @brief 
 * @param db the cart to calculate cost for
 * @return The total cost of all items in shopping cart
 */
void ioopm_calculate_cart_cost(ioopm_db_t *db);

/**
 * @brief adds some item in the database to some shopping cart
 * @param db database of items
 * @param shopping_cart the cart the item is added to
 */
void ioopm_add_to_cart(ioopm_db_t *db);


/**
 * @brief performes a checkout on a shopping cart using its identifier
 * @param db the database where said cart exists
 */
void ioopm_shopping_cart_checkout(ioopm_db_t *db);

option_t calculate_cart_cost(ioopm_db_t *db, int identifier);

bool item_exist_in_cart(ioopm_db_t *db, char *name);

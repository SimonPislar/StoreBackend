#include "common.h"
#include "linked_list.h"
#include "utils.h"
#include "shopping_cart.h"
#include "db.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct ioopm_db
{
    ioopm_hash_table_t *info_ht;
    ioopm_hash_table_t *location_ht;
    ioopm_list_t *shopping_carts;
};

struct item {
    char *name;
    char *desc;
    int price;
};

struct item_info {
    item_t *item;
    ioopm_list_t *location_list;
};

struct location_info
{
    char *name;
    int stock;
};

struct cart_item
{
    char *name;
    int quantity;
    int price;
};

struct shopping_cart
{
    int identifier;
    ioopm_list_t *items;
};

bool compare_cart_items(elem_t a, elem_t b) {
    cart_item_t *cart_a = a.pointer;
    cart_item_t *cart_b = b.pointer;
    if (!strcmp(cart_a->name, cart_b->name)) {
        return true;
    }
    return false;
}

static cart_item_t *cart_item_create(char *name, int quantity, int price)
{
    cart_item_t *cart_item = calloc(1, sizeof(cart_item_t));
    cart_item->name = name;
    cart_item->quantity = quantity;
    cart_item->price = price;
    return cart_item;
}

ioopm_list_t *create_cart_list()
{
    return ioopm_linked_list_create(compare_cart_items); // for the database
}

void shopping_cart_destroy(shopping_cart_t *cart) {
    option_t list_get = ioopm_linked_list_get(cart->items, 0);
    while (list_get.success) {
        cart_item_t *item = list_get.element.pointer;
        free(item);
        ioopm_linked_list_remove(cart->items, 0);
        list_get = ioopm_linked_list_get(cart->items, 0);
    }
    ioopm_linked_list_destroy(cart->items);
    free(cart);
}

void cart_list_destroy(ioopm_list_t *cart_list) {
    int index = 0;
    option_t list_get = ioopm_linked_list_get(cart_list, index);
    while (list_get.success) {
        shopping_cart_t *cart = list_get.element.pointer;
        shopping_cart_destroy(cart);
        index++;
        list_get = ioopm_linked_list_get(cart_list, index);
    }
    ioopm_linked_list_destroy(cart_list);
}


shopping_cart_t *create_shopping_cart(int identifier)
{
    shopping_cart_t *new_cart = calloc(1, sizeof(shopping_cart_t));
    new_cart->items = ioopm_linked_list_create(ioopm_string_equiv_fn); //singular cart in cart list
    new_cart->identifier = identifier;
    return new_cart;
}

option_t find_cart(ioopm_db_t *db, int identifier) {
    ioopm_list_t *cart_list = db->shopping_carts;
    ioopm_list_iterator_t *iterator = ioopm_list_iterator(cart_list);
    option_t get_current = ioopm_iterator_current(iterator);
    while (get_current.success) {
        shopping_cart_t *cart = get_current.element.pointer;
        if (cart->identifier == identifier) {
            ioopm_iterator_destroy(iterator);
            return Success(ptr_elem(cart));
        }
        get_current = ioopm_iterator_next(iterator);
    }
    ioopm_iterator_destroy(iterator);
    return Failure();
}

static option_t remove_cart(ioopm_list_t *cart_list, int identifier)
{
    size_t size = ioopm_linked_list_size(cart_list);
    
    ioopm_list_iterator_t *cart_iter = ioopm_list_iterator(cart_list); 

    option_t current_cart_option = ioopm_iterator_current(cart_iter);
    shopping_cart_t *current_cart = current_cart_option.element.pointer;
    bool current_cart_success = current_cart_option.success;

    for (int i = 0; i < size  ; i++) {
        if(current_cart_success) {
            current_cart = ioopm_iterator_current(cart_iter).element.pointer;
            if(current_cart->identifier == identifier) {
                ioopm_linked_list_remove(cart_list, i);
                shopping_cart_destroy(current_cart);
                ioopm_iterator_destroy(cart_iter);
                return Success(int_elem(identifier));
                }
            current_cart_success = ioopm_iterator_next(cart_iter).success;
            }
        else break;
    }
    ioopm_iterator_destroy(cart_iter);
    printf("Cart with identifier %d does not exist\n",identifier);
    return Failure();
}

option_t ioopm_remove_shopping_cart(ioopm_db_t *db)
{
    int identifier = ask_question_int("Input the identifier of the cart to remove\n");
    printf("Removing cart with identifier %d\n", identifier);
    return remove_cart(db->shopping_carts, identifier);
}

static option_t remove_item_from_cart(shopping_cart_t *cart)
{
    char *name = ask_question_string("input name of item to remove from cart");
    int quantity = ask_question_int("Input quantity to remove");

    ioopm_list_iterator_t *item_iter = ioopm_list_iterator(cart->items);
    option_t current_item_option = ioopm_iterator_current(item_iter);
    cart_item_t *current_item = current_item_option.element.pointer;
    bool current_item_success = current_item_option.success;

    bool success = false;

    size_t size = ioopm_linked_list_size(cart->items);
    for(int i = 0; i < size; i++) {
        current_item = ioopm_iterator_current(item_iter).element.pointer;
        if (current_item_success) {
            if (strcmp(current_item->name, name) == 0) {
                if (current_item->quantity - quantity >= 0) {
                    if (current_item->quantity - quantity == 0) {
                        free(current_item);
                        ioopm_linked_list_remove(cart->items, i);
                        success = true; 
                        }
                    else current_item->quantity = current_item->quantity - quantity;
                    
                    printf("Removing %d %s from cart",quantity, name);
                    free(name);
                    ioopm_iterator_destroy(item_iter);
                    return Success(ptr_elem(name));
                    }
                else break;    
                }
                else current_item_success = ioopm_iterator_next(item_iter).success;              
                }
        else break;
    }
    if (!success) {
        printf("%s Does Not exist in wanted cart", name);
    }
    free(name);
    ioopm_iterator_destroy(item_iter);
    return Failure();
    
}

option_t calculate_cart_cost(ioopm_db_t *db, int identifier)
{
    option_t get_cart = find_cart(db, identifier);
    if (get_cart.success) {
        int total_cost = 0;
        shopping_cart_t *cart = get_cart.element.pointer;
        size_t size = ioopm_linked_list_size(cart->items);
        for (int i = 0; i < size; i++) {
            option_t get_item = ioopm_linked_list_get(cart->items, i);
            if (get_item.success) {
                cart_item_t *cart_item = get_item.element.pointer;
                total_cost += (cart_item->price) * cart_item->quantity;
            } else {
                printf("Item not reachable in cart for some reason.\n");
            }
        }
        return Success(int_elem(total_cost));
    } else {
        printf("Cart not found in list.\n");
        return Failure();
    }
}
option_t ioopm_remove_from_cart(ioopm_db_t *db)
{
    int identifier = ask_question_int("Input cart identifier");
    option_t find_cart_option = find_cart(db, identifier);
    if (find_cart_option.success) {
        return remove_item_from_cart(find_cart_option.element.pointer);
    }
    else {
        puts("No cart with that identifier");
        return Failure();
    }
}



void ioopm_calculate_cart_cost(ioopm_db_t *db) {
    int answer = ask_question_int("Identifier for cart: ");
    option_t get_cost = calculate_cart_cost(db, answer);
    if (get_cost.success) {
        int cost = get_cost.element.integer;
        printf("Cart cost: %d.%d\n SEK", cost/100, cost % 100);
    } else {
        printf("A cart with that identifier doesn't exist");
    }
}


static void add_merch_to_cart(char *name_of_merch, ioopm_hash_table_t *ht, shopping_cart_t *shopping_cart, int quantity) {
    item_info_t *item_info = ioopm_hash_table_lookup(ht, ptr_elem(name_of_merch)).element.pointer;
    cart_item_t *cart_item = cart_item_create(item_info->item->name, quantity, item_info->item->price);
    ioopm_linked_list_append(shopping_cart->items, ptr_elem(cart_item));
}

static int count_stock_of_merch_in_cart(shopping_cart_t *shopping_cart, char *name_of_merch) {
    ioopm_list_t *list_of_merch = shopping_cart->items;
    size_t list_length = ioopm_linked_list_size(list_of_merch);
    int stock = 0;
    for (int index = 0; index < list_length; index++) {
        cart_item_t *current_cart_item = ioopm_linked_list_get(list_of_merch, index).element.pointer;
        if (!strcmp(current_cart_item->name, name_of_merch)) {
            stock += current_cart_item->quantity;
        }
    }
    return stock;
}

static int reserved_stock(ioopm_db_t *db, char *name_of_merch) {
    ioopm_list_t *list_of_carts = db->shopping_carts;
    int reserved_stock_count = 0;
    size_t list_length = ioopm_linked_list_size(list_of_carts);
    for (int index = 0; index < list_length; index++) {
        shopping_cart_t *current_cart = ioopm_linked_list_get(list_of_carts, index).element.pointer;
        reserved_stock_count += count_stock_of_merch_in_cart(current_cart, name_of_merch);
    }
    return reserved_stock_count;
}

bool check_stock_count_available(ioopm_db_t *db, int quantity, char *name_of_merch) {
    int stock_count = count_stock(db, name_of_merch);
    if ((stock_count - reserved_stock(db, name_of_merch)) < quantity) {
        return false;
    }
    return true;
}

void ioopm_add_to_cart(ioopm_db_t *db) {
    printf("Adding item to cart...\n");
    int identifier = ask_question_int("Input cart identifier: ");
    char *merch = ask_question_string("Name of item to add to the cart: ");

    option_t find_cart_option = find_cart(db, identifier);
    if (find_cart_option.success) {
        shopping_cart_t *shopping_cart = find_cart_option.element.pointer;

    option_t look_up = ioopm_hash_table_lookup(db->info_ht, ptr_elem(merch));
    if (look_up.success) {
        int quantity = ask_question_int("Amount of that item to add: ");
        if (check_stock_count_available(db, quantity, merch)) {
            add_merch_to_cart(merch, db->info_ht, shopping_cart, quantity);
            printf("Adding %d %s to cart\n", quantity, merch);
        } else {
            printf("Not enough stock available!\n");
        }
    } else {
        printf("Item named %s doesn't exist.\n", merch);
        }
    }
    else puts("Cart with this identifier does not exist\n");
    free(merch);
}


static void checkout_item(ioopm_db_t *db, cart_item_t *item)
{
    item_info_t *item_info = ioopm_hash_table_lookup(db->info_ht, ptr_elem(item->name)).element.pointer;
    ioopm_list_t *item_locations = item_info->location_list;
    char *current_location = ioopm_linked_list_get(item_locations, 0).element.pointer;
    
    location_info_t *to_remove_from = ioopm_hash_table_lookup(db->location_ht, ptr_elem(current_location)).element.pointer;
    if(item->quantity < to_remove_from->stock) {
        to_remove_from->stock = to_remove_from->stock - item->quantity;
        return;
        }
    else if (item->quantity == to_remove_from->stock) {
        ioopm_hash_table_remove(db->location_ht, ptr_elem(current_location));
        free(to_remove_from);
        free(ioopm_linked_list_get(item_locations, 0).element.pointer);
        ioopm_linked_list_remove(item_locations, 0);
        return;
        }
    else {
        ioopm_hash_table_remove(db->location_ht, ptr_elem(current_location));
        free(ioopm_linked_list_get(item_locations, 0).element.pointer);
        ioopm_linked_list_remove(item_locations, 0);
        item->quantity = item->quantity - to_remove_from->stock;
        checkout_item(db, item);
        }
    free(to_remove_from);     
}

static void checkout_items(ioopm_db_t *db, shopping_cart_t *cart)
{
    ioopm_list_iterator_t *item_iter = ioopm_list_iterator(cart->items);
    if(ioopm_iterator_current(item_iter).success) {
        option_t current_item_option = ioopm_iterator_current(item_iter);
        bool current_item_success = current_item_option.success;
        
        while(current_item_success) {
            checkout_item(db, ioopm_iterator_current(item_iter).element.pointer);
            current_item_success = ioopm_iterator_next(item_iter).success;
        }
    }
    ioopm_iterator_destroy(item_iter);
}


void ioopm_shopping_cart_checkout(ioopm_db_t *db)
{
    int cart_id = ask_question_int("Input cart number");
    ioopm_list_t *cart_list = db->shopping_carts;
    ioopm_list_iterator_t *cart_iter = ioopm_list_iterator(cart_list);
    
    option_t current_cart_option = ioopm_iterator_current(cart_iter);
    shopping_cart_t *current_cart = current_cart_option.element.pointer;
    bool current_cart_success = current_cart_option.success;

    while(current_cart_success) {
        current_cart = ioopm_iterator_current(cart_iter).element.pointer;
        if (current_cart->identifier == cart_id) {
            checkout_items(db, current_cart);
            break;
            }
        else current_cart_success = ioopm_iterator_next(cart_iter).success;
    }
    if (current_cart) {
        printf("Cart %d is checked out", cart_id);
        remove_cart(cart_list, cart_id);
    }
    else puts("No cart with that cart number");

    ioopm_iterator_destroy(cart_iter);
}

bool item_exist_in_cart(ioopm_db_t *db, char *name) {
    ioopm_list_t *cart_list = db->shopping_carts;
    size_t cart_list_length = ioopm_linked_list_size(cart_list);
    for (int i = 0; i < cart_list_length; i++) {
        shopping_cart_t *cart = ioopm_linked_list_get(cart_list, i).element.pointer;
        ioopm_list_t *cart_items = cart->items;
        size_t cart_items_length = ioopm_linked_list_size(cart_items);
        for (int j = 0; j < cart_items_length; j++) {
            cart_item_t *cart_item = ioopm_linked_list_get(cart_items, j).element.pointer;
            if (!strcmp(cart_item->name, name)) {
                return true;
            }
        }
    }
    return false;
}
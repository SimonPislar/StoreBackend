#include "shopping_cart.h"
#include <CUnit/Basic.h>
#include "db.h"

struct cart_item 
{
    char *name;
    int quantity;
    int price;
};

struct ioopm_db
{
    ioopm_hash_table_t *info_ht;
    ioopm_hash_table_t *location_ht;
    ioopm_list_t *shopping_carts;
    int cart_counter;
};

struct shopping_cart
{
    int identifier;
    ioopm_list_t *items;
};

struct link 
{
    elem_t element;
    link_t *next;
};

struct list 
{
    struct link *first;
    struct link *last;
    ioopm_eq_function eq_fn;
};

struct location_info
{
    char *name;
    int stock;
};

struct item_info {
    item_t *item;
    ioopm_list_t *location_list;
};


int init_suite(void) {
  // Change this function if you want to do something *before* you
  // run a test suite
  return 0;
}

int clean_suite(void) {
  // Change this function if you want to do something *after* you
  // run a test suite
  return 0;
}


void test_cart_create_destroy()
{
    shopping_cart_t *cart = create_shopping_cart(1);
    CU_ASSERT_PTR_NOT_NULL(cart);
    shopping_cart_destroy(cart);
}



void test_cart_remove()
{   
    ioopm_db_t *db = ioopm_db_create();
    ioopm_db_add_cart(db);
    ioopm_db_add_cart(db);
    ioopm_db_add_cart(db);
    CU_ASSERT_EQUAL(ioopm_remove_shopping_cart(db).element.integer,2);
    CU_ASSERT_EQUAL(ioopm_linked_list_size(db->shopping_carts), 2);
    CU_ASSERT_EQUAL(ioopm_remove_shopping_cart(db).element.integer,1);
    CU_ASSERT_EQUAL(ioopm_linked_list_size(db->shopping_carts), 1);
    CU_ASSERT_EQUAL(ioopm_remove_shopping_cart(db).element.integer,3);
    CU_ASSERT_EQUAL(ioopm_linked_list_size(db->shopping_carts), 0);
    ioopm_db_destroy(db);
}


void test_remove_from_cart()
{
    ioopm_db_t *db = ioopm_db_create();
    ioopm_db_add_cart(db);
    shopping_cart_t *cart1 = db->shopping_carts->first->element.pointer;
    ioopm_db_insert(db); //add name banana, desc fruit, price 40, shelf B99, quantity 50
    ioopm_db_insert(db); //add name apple, desc fruit, price 30, shelf G63, quantity 30
    ioopm_db_insert(db); //add name gurka, desc fruit, price 10, shelf A23, quantity 20
    ioopm_add_to_cart(db); //add 20 apple cart 1
    ioopm_add_to_cart(db); //add 40 banana cart 1
    ioopm_add_to_cart(db); //add 10 gurka cart 1
    
    CU_ASSERT_FALSE(ioopm_remove_from_cart(db).success); //remove from cart 1, 31 apples
    CU_ASSERT_FALSE(ioopm_remove_from_cart(db).success); // remove from cart 1, 1 melon

    ioopm_remove_from_cart(db); // remove from cart 1, 10 apples
    cart_item_t *apple_item = cart1->items->first->element.pointer;
    CU_ASSERT_EQUAL(apple_item->quantity, 10);
    CU_ASSERT_STRING_EQUAL(apple_item->name, "apple");

    ioopm_remove_from_cart(db); // remove 30 bananas
    cart_item_t *banana_item = cart1->items->first->next->element.pointer;
    CU_ASSERT_EQUAL(banana_item->quantity, 10);
    CU_ASSERT_STRING_EQUAL(banana_item->name, "banana");

    ioopm_remove_from_cart(db); // remove 10 gurka
    CU_ASSERT_FALSE(ioopm_remove_from_cart(db).success); // remove 1 gurka
    CU_ASSERT_EQUAL(ioopm_linked_list_size(cart1->items), 2);
    ioopm_db_destroy(db);
}

void test_calculate_cart_cost()
{
    ioopm_db_t *db = ioopm_db_create();
    ioopm_db_add_cart(db);
    ioopm_db_add_cart(db);
    ioopm_db_add_cart(db);
    // shopping_cart_t *cart1 = db->shopping_carts->first->element.pointer;
    // shopping_cart_t *cart2 = db->shopping_carts->first->next->element.pointer;

    ioopm_db_insert(db); //add name banana, desc fruit, price 15, shelf B99, quantity 100
    ioopm_db_insert(db); //add name apple, desc fruit, price 30, shelf G63, quantity 100
    ioopm_db_insert(db); //add name gurka, desc fruit, price 10, shelf A23, quantity 100

    ioopm_add_to_cart(db); //add 5 apple
    ioopm_add_to_cart(db); //add 5 apple
    ioopm_add_to_cart(db); //add 10 banana
    ioopm_add_to_cart(db); //add 1 gurka
    option_t cart1Option = calculate_cart_cost(db, 1);
    option_t cart2Option = calculate_cart_cost(db, 2);
    option_t cart3Option = calculate_cart_cost(db, 3);
    CU_ASSERT_TRUE_FATAL(cart1Option.success);
    CU_ASSERT_TRUE_FATAL(cart2Option.success);
    CU_ASSERT_TRUE_FATAL(cart3Option.success);
    CU_ASSERT_EQUAL(cart1Option.element.integer, 150);
    CU_ASSERT_EQUAL(cart2Option.element.integer, 310);
    CU_ASSERT_EQUAL(cart3Option.element.integer, 0);

    ioopm_db_destroy(db);
}

void test_add_to_cart()
{
    ioopm_db_t *db = ioopm_db_create();
    ioopm_db_add_cart(db);
    shopping_cart_t *cart1 = ioopm_linked_list_get(db->shopping_carts, 0).element.pointer;
    ioopm_db_insert(db); //add name banana, desc fruit, price 40, shelf B99, quantity 3
    ioopm_db_insert(db); //add name apple, desc fruit, price 30, shelf G63, quantity 114
    ioopm_add_to_cart(db); //add banana 
    ioopm_add_to_cart(db); //add apple
    cart_item_t *banana = ioopm_linked_list_get(cart1->items, 0).element.pointer;
    cart_item_t *apple = ioopm_linked_list_get(cart1->items, 1).element.pointer;
    char *name_banana = banana->name;
    char *name_apple = apple->name;
    CU_ASSERT_STRING_EQUAL(name_apple, "apple");
    CU_ASSERT_STRING_EQUAL(name_banana, "banana");
    ioopm_db_destroy(db);
}


void test_cart_checkout()
{
    ioopm_db_t *db = ioopm_db_create();
    ioopm_db_add_cart(db);
    ioopm_db_add_cart(db);
    ioopm_db_add_cart(db);
    // shopping_cart_t *cart1 = db->shopping_carts->first->element.pointer;
    // shopping_cart_t *cart2 = db->shopping_carts->first->next->element.pointer;
    

    ioopm_db_insert(db); //add name apple, desc fruit, price 30, shelf G63, quantity 10
    ioopm_db_insert(db); //add name banana, desc fruit, price 15, shelf B99, quantity 5
    ioopm_db_replenish(db); // add 6 bananas to B90
    ioopm_db_insert(db); //add name gurka, desc fruit, price 10, shelf A23, quantity 15

    ioopm_add_to_cart(db); //add 5 apple cart 1
    ioopm_add_to_cart(db); //add 5 apple cart 2 
    ioopm_add_to_cart(db); //add 10 banana cart 2
    ioopm_add_to_cart(db); //add 10 gurka cart 2

    ioopm_shopping_cart_checkout(db); //checkout cart 2
    location_info_t *A23 = ioopm_hash_table_lookup(db->location_ht, ptr_elem("A23")).element.pointer;
    int A23_quantity = A23->stock;
    CU_ASSERT_EQUAL(A23_quantity,5);

    item_info_t *banana_info = ioopm_hash_table_lookup(db->info_ht, ptr_elem("banana")).element.pointer;
    CU_ASSERT_EQUAL(ioopm_linked_list_size(banana_info->location_list),1);
    char *shelf_number = banana_info->location_list->first->element.pointer;
    location_info_t *banana_shelf = ioopm_hash_table_lookup(db->location_ht, ptr_elem(shelf_number)).element.pointer;
    int banana_shelf_quantity = banana_shelf->stock;
    CU_ASSERT_EQUAL(banana_shelf_quantity,1);



    ioopm_shopping_cart_checkout(db); //checkout cart 1
    item_info_t *apple_info = ioopm_hash_table_lookup(db->info_ht, ptr_elem("apple")).element.pointer;
    CU_ASSERT_EQUAL(ioopm_linked_list_size(apple_info->location_list),0);


    ioopm_shopping_cart_checkout(db); //checkout cart 3 

    ioopm_db_destroy(db);
}




int main() {
    // Try to set up CUnit, exit if we fail
    if (CU_initialize_registry() != CUE_SUCCESS)
        return CU_get_error();

    // Create an empty test suite and specify the name and
    // the init and cleanup functions
    CU_pSuite my_test_suite = CU_add_suite("Test suite for hash tables", init_suite, clean_suite);
    if (my_test_suite == NULL) {
        // If the test suite could not be added, tear down CUnit and exit
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (
            (CU_add_test(my_test_suite, "test cart create destroy", test_cart_create_destroy) == NULL) ||
            (CU_add_test(my_test_suite, "test cart remove", test_cart_remove) == NULL) ||
            (CU_add_test(my_test_suite, "test remove from cart", test_remove_from_cart) == NULL) ||
            (CU_add_test(my_test_suite, "test calculate cart cost", test_calculate_cart_cost) == NULL) ||
            (CU_add_test(my_test_suite, "test add to cart", test_add_to_cart) == NULL) ||
            (CU_add_test(my_test_suite, "test cart checkout", test_cart_checkout) == NULL) ||
            0)
    {
        // If adding any of the tests fails, we tear down CUnit and exit
        CU_cleanup_registry();
        return CU_get_error();
    }
    // Set the running mode. Use CU_BRM_VERBOSE for maximum output.
    // Use CU_BRM_NORMAL to only print errors and a summary
    CU_basic_set_mode(CU_BRM_VERBOSE);

    // This is where the tests are actually run!
    CU_basic_run_tests();

    // Tear down CUnit before exiting
    CU_cleanup_registry();
    return CU_get_error();
}

#include "merch.h"
#include <CUnit/Basic.h>
#include "db.h"

struct ioopm_db 
{
    ioopm_hash_table_t *info_ht;
    ioopm_hash_table_t *location_ht;
};

struct item
{
    char *name;
    char *desc;
    int price;
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

void test_add_merchandise() 
{
    ioopm_db_t *db = ioopm_db_create();
    ioopm_db_insert(db); // name = hej, desc = tjena, price = 100 shelf = A23, stock = 10
    item_info_t *wanted_item = (ioopm_hash_table_lookup(db->info_ht, ptr_elem("hej")).element).pointer;
    option_t shelf_lookup = ioopm_hash_table_lookup(db->location_ht, ptr_elem("A23"));
    CU_ASSERT_STRING_EQUAL("tjena", wanted_item->item->desc);
    CU_ASSERT_EQUAL(100, wanted_item->item->price);
    CU_ASSERT_TRUE(shelf_lookup.success);
    location_info_t *location_info = shelf_lookup.element.pointer; //segfault om man skriver fel hylla
    char *merch_name = location_info->name;
    CU_ASSERT_STRING_EQUAL("hej", merch_name);
    CU_ASSERT_EQUAL(10, location_info->stock);
    ioopm_db_destroy(db);
}

void test_remove_merchandise()
{
    ioopm_db_t *db = ioopm_db_create();
    ioopm_db_insert(db); // name = hej, desc = tjena, price = 100 shelf = A23, stock = 10
    CU_ASSERT_TRUE(ioopm_hash_table_lookup(db->info_ht, ptr_elem("hej")).success);
    CU_ASSERT_TRUE(ioopm_hash_table_lookup(db->location_ht, ptr_elem("A23")).success);
    ioopm_db_remove(db); //remove hej
    CU_ASSERT_FALSE(ioopm_hash_table_lookup(db->info_ht, ptr_elem("hej")).success);
    CU_ASSERT_FALSE(ioopm_hash_table_lookup(db->location_ht, ptr_elem("A23")).success);
    ioopm_db_insert(db); // name = hejsan, desc = tja, price = 30, shelf = B23, stock = 10
    ioopm_db_replenish(db); // name = hejsan, shelf = C23 new_stock = 25
    //TODO: fix replenish added shelf not being removed by db_remove.
    CU_ASSERT_TRUE(ioopm_hash_table_lookup(db->info_ht, ptr_elem("hejsan")).success);
    CU_ASSERT_TRUE(ioopm_hash_table_lookup(db->location_ht, ptr_elem("B23")).success);
    CU_ASSERT_TRUE(ioopm_hash_table_lookup(db->location_ht, ptr_elem("C23")).success);
    ioopm_db_remove(db); // remove hejsan
    CU_ASSERT_FALSE(ioopm_hash_table_lookup(db->info_ht, ptr_elem("hejsan")).success);
    CU_ASSERT_FALSE(ioopm_hash_table_lookup(db->location_ht, ptr_elem("B23")).success);
    CU_ASSERT_FALSE(ioopm_hash_table_lookup(db->location_ht, ptr_elem("C23")).success);
    ioopm_db_destroy(db);
}

void test_edit_item()
{
    ioopm_db_t *db = ioopm_db_create();
    ioopm_db_insert(db); // name = hej, desc = tjena, price = 100, Shelf = A23, Stock = 10
    ioopm_db_replenish(db); // name = hej, Shelf = B23, Stock = 15

    item_info_t *item_info = (ioopm_hash_table_lookup(db->info_ht, ptr_elem("hej")).element.pointer);
    ioopm_list_t *shelf_list = item_info->location_list;
    ioopm_list_iterator_t *shelf_list_iter = ioopm_list_iterator(shelf_list);
    size_t size = ioopm_linked_list_size(shelf_list);

    ioopm_db_edit_item(db); //name hej desc tjena => tjenare, price 100 => 150
    item_info = (ioopm_hash_table_lookup(db->info_ht, ptr_elem("hej")).element.pointer);
    CU_ASSERT_EQUAL(150, item_info->item->price);
    CU_ASSERT_STRING_EQUAL("tjenare", item_info->item->desc);

    ioopm_db_edit_item(db); //name hej => hejsan
    CU_ASSERT_TRUE(ioopm_hash_table_lookup(db->info_ht, ptr_elem("hejsan")).success);
    CU_ASSERT_FALSE(ioopm_hash_table_lookup(db->info_ht, ptr_elem("hej")).success);

    location_info_t *loc;

    for(int i = 0; i < size; i++) {
        loc = ioopm_hash_table_lookup(db->location_ht, ioopm_iterator_current(shelf_list_iter).element).element.pointer;
        printf("loc->name = %s\n", loc->name);
        CU_ASSERT_STRING_EQUAL(loc->name, "hejsan");
        ioopm_iterator_next(shelf_list_iter);
    }

    ioopm_iterator_destroy(shelf_list_iter);
    ioopm_db_destroy(db);
}

void test_replenish_existing_shelf() {
    ioopm_db_t *db = ioopm_db_create();
    ioopm_db_insert(db); //add name = hej, desc = desc  shelf = A13, stock = 1,
    item_info_t *item_info = (ioopm_hash_table_lookup(db->info_ht, ptr_elem("hej")).element.pointer);
    location_info_t *location_info = ioopm_hash_table_lookup(db->location_ht, ptr_elem("A13")).element.pointer;
    CU_ASSERT_STRING_EQUAL(item_info->item->name, "hej");
    CU_ASSERT_EQUAL(location_info->stock, 1);
    ioopm_db_replenish(db); //increase stock of item hej on shelf A13 with 2
    CU_ASSERT_EQUAL(location_info->stock, 3);
    CU_ASSERT_STRING_EQUAL(item_info->item->name, "hej");
    ioopm_db_destroy(db);
}

void test_replenish_non_existing_shelf() {
    ioopm_db_t *db = ioopm_db_create();
    ioopm_db_insert(db); //add name = hej, desc=desc,  shelf = A13, stock 1,
    item_info_t *item_info = (ioopm_hash_table_lookup(db->info_ht, ptr_elem("hej")).element.pointer);
    location_info_t *location_info_A13 = ioopm_hash_table_lookup(db->location_ht, ptr_elem("A13")).element.pointer;
    CU_ASSERT_STRING_EQUAL(item_info->item->name, "hej");
    CU_ASSERT_EQUAL(location_info_A13->stock, 1);
    ioopm_db_replenish(db); //increase stock of item hej on shelf A14 with 2
    CU_ASSERT_EQUAL(location_info_A13->stock, 1);
    location_info_t *location_info_A14 = ioopm_hash_table_lookup(db->location_ht, ptr_elem("A14")).element.pointer;
    CU_ASSERT_STRING_EQUAL(item_info->item->name, "hej");
    CU_ASSERT_EQUAL(location_info_A14->stock, 2);
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
            (CU_add_test(my_test_suite, "test add merch", test_add_merchandise) == NULL) ||
            (CU_add_test(my_test_suite, "test remove merch", test_remove_merchandise) == NULL) ||
            (CU_add_test(my_test_suite, "test edit merchandise", test_edit_item) == NULL) ||
            (CU_add_test(my_test_suite, "test replenish existing shelf", test_replenish_existing_shelf) == NULL) ||
            (CU_add_test(my_test_suite, "test replenish non-existing shelf", test_replenish_non_existing_shelf) == NULL) ||
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

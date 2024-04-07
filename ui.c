#include "utils.h"
#include "db.h"
#include "merch.h"
#include "shopping_cart.h"

//#define TEST


struct item {
    char *name;
    char *desc;
    int price;
};

struct cart_item
{
    char *name;
    int quantity;
    int price;
};

struct item_info {
    item_t *item;
    ioopm_list_t *location_list;
};

struct ioopm_db 
{
    ioopm_hash_table_t *info_ht;
    ioopm_hash_table_t *location_ht;
    ioopm_list_t *shopping_carts;
    int cart_counter;
};

struct location_info
{
    char *name;
    int stock;
};


void print_list_of_merchandise(size_t length_of_merch, char** merch_arr) {
    int merch_index = 0;
    int answer = 2;
    char *continue_question = "\nPrint next 20 items?"
                              "[0] Yes"
                              "[1] No";
    do {
        for (merch_index = 0; merch_index < 20 && merch_index < length_of_merch; merch_index++) {
            printf("\n%d. %s", (merch_index + 1), merch_arr[merch_index]);
        }
        if (merch_index < length_of_merch) {
            answer = ask_question_binary(continue_question);
        }
    } while (answer == 0);
    printf("\n");
}

void edit_item_menu(ioopm_db_t *db, item_info_t *item_to_change)
{
    while (true) {
        int choice = ask_question(
                    "\nChoose action\n"
                    "\n[1] Edit name\n"
                    "[2] Edit description\n"
                    "[3] Edit price\n"
                    "[4] Go back\n",
                    (check_func) is_edit_menu_choice,
                    (convert_func) atoi).int_value;

        if (choice == 1) {
            edit_item_name(db, item_to_change);
        }
        if (choice == 2) {
            char *new_desc = ask_question_string("Input new description: ");
            char *to_free = item_to_change->item->desc;
            item_to_change->item->desc = new_desc;
            free(to_free);
            puts("\nChanged item:\n");
            print_item(item_to_change->item);
        }
        if (choice == 3) {
            int new_price = -10; 
            while(new_price < 1) {
            new_price = ask_question_int("Input new price: ");
            item_to_change->item->price = new_price;
            }
            puts("\nChanged item:\n");
            print_item(item_to_change->item);
        }
        if (choice == 4) {
            break;
        }
    }
}


void static cart_menu_loop(ioopm_db_t *db)
{
    while (true) {
        int choice = ask_question(
                    "\n[1] Add cart\n"
                    "[2] Remove cart\n"
                    "[3] Add to cart\n"
                    "[4] Remove From cart\n"
                    "[5] Calculate cart cost\n"
                    "[6] Checkout cart\n"
                    "[7] Go back\n",
                    (check_func) is_number,
                    (convert_func) atoi).int_value;
        switch(choice) {
        case 1 : 
            ioopm_db_add_cart(db);
            break;
        case 2 : 
            ioopm_remove_shopping_cart(db);
            break;
        case 3 :
            ioopm_add_to_cart(db);
            break;
        case 4 :
            ioopm_remove_from_cart(db);
            break;
        case 5 :
            ioopm_calculate_cart_cost(db);
            break;
        case 6 :
            ioopm_shopping_cart_checkout(db);
            break;
        case 7 :
            puts("");
            return;
        }
    }
}

void static menu_loop(ioopm_db_t *db) 
{
    while (true) {
        int choice = ask_question(
                    "\n[1] Add Merchendise\n"
                    "[2] List Merchendise\n"
                    "[3] Remove Merchendise\n"
                    "[4] Edit Merchendise\n"
                    "[5] Show Stock\n"
                    "[6] Replenish Stock\n"
                    "[7] Cart menu\n"
                    "[8] Quit\n",
                    (check_func) is_number,
                    (convert_func) atoi).int_value;
        switch(choice) {
        case 1 : 
            ioopm_db_insert(db);
            break;
        case 2 : 
            ioopm_list_merchandise(db);
            break;
        case 3 :
            ioopm_db_remove(db);
            break;
        case 4 :
            ioopm_db_edit_item(db);
            break;
        case 5 :
            ioopm_show_stock(db);
            break;
        case 6 :
            ioopm_db_replenish(db);
            break;
        case 7 :
            cart_menu_loop(db);
            break;
        case 8 :
            puts("Quitting...");
            return; 
        }
    }
}

#ifndef TEST
int main()
{
    ioopm_db_t *db = ioopm_db_create();
    menu_loop(db);
    ioopm_db_destroy(db);
    return 0;
}
#endif
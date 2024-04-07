#include "utils.h"
#include "hash_table.h"
#include "merch.h"
#include "db.h"
#include "common.h"
#include "shopping_cart.h"
#include "ui.h"



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

static void destroy_shelf_elem(elem_t ignored_index, elem_t *shelf_elem, void *ignored_extra) {
    char *shelf_to_destroy = shelf_elem->pointer;
    free(shelf_to_destroy);
}

static void destroy_item_info_t(item_info_t *item_info) {
    ioopm_item_destroy(item_info->item); //this and line below frees the same thing sometimes
    ioopm_linked_list_apply_to_all(item_info->location_list, destroy_shelf_elem, NULL);
    ioopm_linked_list_destroy(item_info->location_list);
    free(item_info);
}

bool ioopm_location_info_equiv_fn (elem_t a, elem_t b) {
    location_info_t *a_info = a.pointer;
    location_info_t *b_info = b.pointer;

    if (strcmp(a_info->name, b_info->name) == 0 && a_info->stock == b_info->stock) {
        return true;
    }
    return false;
}


ioopm_db_t *ioopm_db_create()
{
    ioopm_db_t *db = calloc(1, sizeof(ioopm_db_t));
    db->info_ht = ioopm_hash_table_create((ioopm_hash_function) string_knr_hash, NULL, ioopm_string_equiv_fn); //Maps name of item (char *) to item_info_t
    db->location_ht = ioopm_hash_table_create((ioopm_hash_function) string_knr_hash, ioopm_location_info_equiv_fn, ioopm_string_equiv_fn);
    db->shopping_carts = create_cart_list();
    db->cart_counter = 0;
    return db;
}


void ioopm_db_add_cart(ioopm_db_t *db)
{
    db->cart_counter++;
    printf("Adding Shopping cart with identifier %d\n", db->cart_counter);
    shopping_cart_t *new_cart = create_shopping_cart(db->cart_counter);
    ioopm_linked_list_append(db->shopping_carts, ptr_elem(new_cart));
}

void static destroy_current_item_info_t(elem_t key_ignored, elem_t *item_info_elem, void *extra)
{
    item_info_t *item_info_to_destroy = item_info_elem->pointer;
    destroy_item_info_t(item_info_to_destroy);
}

void static destroy_current_location_info_t(elem_t key, elem_t *location_info_elem, void *extra)
{
    location_info_t *location_info_to_destroy = location_info_elem->pointer;
    free(location_info_to_destroy);
}

item_info_t *create_item_info() {
    item_info_t *item_info = malloc(sizeof(item_info_t));
    item_info->item = input_item();
    item_info->location_list = ioopm_linked_list_create(ioopm_string_equiv_fn);
    return item_info;
}

void ioopm_db_destroy(ioopm_db_t *db)
{   
    ioopm_hash_table_apply_to_all(db->info_ht, destroy_current_item_info_t, NULL);
    ioopm_hash_table_apply_to_all(db->location_ht, destroy_current_location_info_t, NULL);
    ioopm_hash_table_destroy(db->info_ht); //destroy info_ht
    ioopm_hash_table_destroy(db->location_ht); //destroy location_ht
    cart_list_destroy(db->shopping_carts); // destroys all carts
    free(db);
}

location_info_t *create_location_info(item_info_t *item_info) {
    location_info_t *info = calloc(1, sizeof(location_info_t)); //allocates location_info on the heap
    int amount_of_stock = ask_question("How much stock at this shelf?",is_bigger_than_zero, 
                                                                        (answer_t (*)(char *))atoi).int_value;
    info->stock = amount_of_stock; //sets stock to wanted amount
    info->name = item_info->item->name; //sets the item name to item_info's name
    return info;
}

bool static check_item_existance(ioopm_db_t *db, char *name) {
    option_t look_up = ioopm_hash_table_lookup(db->info_ht, ptr_elem(name));
    return look_up.success;
}

bool static check_existence_of_item_before_insert(ioopm_db_t *db, item_info_t *new_item_info, char *shelf) {
    if (ioopm_hash_table_lookup(db->info_ht, ptr_elem(new_item_info->item->name)).success) { //checks if the item already exists
        puts("This item does already exist");
        return true;
    }
    if (ioopm_hash_table_lookup(db->location_ht, ptr_elem(shelf)).success) { //checks if the shelf is free to use
        puts("This location is already taken!");
        return true;
    }
    return false;
}

void static linked_list_to_static_char_array(char** buf, ioopm_list_iterator_t *iterator) {
    int counter = 0;
    option_t current = ioopm_iterator_current(iterator);
    do {
        char *value = current.element.pointer;
        buf[counter] = value;
        counter++;
        current = ioopm_iterator_next(iterator);
    } while (current.success);
}

option_t get_db_item_array(ioopm_db_t *db) {
    ioopm_list_t *merch = ioopm_hash_table_keys(db->info_ht);
    ioopm_list_iterator_t *iterator = ioopm_list_iterator(merch);
    size_t length_of_merch = ioopm_linked_list_size(merch);
    char **merch_arr = calloc(1, sizeof(char *) * length_of_merch);
    if (length_of_merch < 1) {
        free(merch_arr);
        ioopm_linked_list_destroy(merch);
        ioopm_iterator_destroy(iterator);
        return Failure();
    }
    linked_list_to_static_char_array(merch_arr, iterator);
    ioopm_iterator_destroy(iterator);
    ioopm_linked_list_destroy(merch);
    qsort(merch_arr, length_of_merch, sizeof(char *), string_compare);
    return Success(ptr_elem(merch_arr));
}

void ioopm_list_merchandise(ioopm_db_t *db) {
    size_t length_of_merch = ioopm_hash_table_size(db->info_ht);
    option_t item_array = get_db_item_array(db);
    if (item_array.success) {
        char **merch_arr = item_array.element.pointer;
        print_list_of_merchandise(length_of_merch, merch_arr);
        free(merch_arr);
    } else {
        printf("Database is currently empty.\n");
    }
}

void ioopm_db_insert(ioopm_db_t *db) {
    item_info_t *new_item_info = create_item_info(); //creates a new item_info_t with input from user
    char *shelf = ask_question_shelf("Shelf?"); //Asks the user for the shelf where the item is to be stored
    if (!check_existence_of_item_before_insert(db, new_item_info, shelf)) {
        //now we know that this item and shelf is legal, so we proceed by adding it to the db
        ioopm_linked_list_append(new_item_info->location_list, ptr_elem(shelf)); //adds the shelf to the location_list
        ioopm_hash_table_insert(db->info_ht, ptr_elem(new_item_info->item->name), ptr_elem(new_item_info)); //adds the items information to the db and maps it to the item's name
        location_info_t *info = create_location_info(new_item_info); //creates location_info from the item we just created
        ioopm_hash_table_insert(db->location_ht, ptr_elem(shelf), ptr_elem(info)); //adds the location_info to the db and maps it to the shelf's name
    } else {
        printf("\nItem with that name already exist.\n");
        destroy_item_info_t(new_item_info); //destroys the created item
        free(shelf);
    }
}

void static apply_to_all_change_name(elem_t key_ignored, elem_t *value, void *names)
{
    location_info_t *loc = value->pointer;

    char **name_arr = ((char **) names);
    
    if (strcmp(loc->name, name_arr[0]) == 0) {
        loc->name = name_arr[1];
    }
}

void edit_item_name(ioopm_db_t *db, item_info_t *item_to_change)
{
    char *new_name = ask_question_string("Input new name: ");
    if (! ioopm_hash_table_lookup(db->info_ht, ptr_elem(new_name)).success) {

        char *old_name = item_to_change->item->name;
        ioopm_hash_table_remove(db->info_ht, ptr_elem(item_to_change->item->name));
        item_to_change->item->name = new_name;
        ioopm_hash_table_insert(db->info_ht, ptr_elem(item_to_change->item->name), ptr_elem(item_to_change));

        char *names[2] = {old_name, new_name};

        ioopm_hash_table_apply_to_all(db->location_ht, apply_to_all_change_name, &names);
        free(old_name);

        puts("\nChanged item:\n");
        print_item(item_to_change->item);
    } else {
        puts("Item of that name already exists");
        free(new_name);
    }
}

void ioopm_db_edit_item(ioopm_db_t *db)
{
    option_t op_merch_arr = get_db_item_array(db);
    if (op_merch_arr.success) {
        char **merch_arr = op_merch_arr.element.pointer;
        size_t size = ioopm_hash_table_size(db->info_ht);
        print_list_of_merchandise(size, merch_arr);
        int index_of_merch = ask_question_int("Index of item: ");
        char *item_to_change = merch_arr[index_of_merch - 1];
        if (item_exist_in_cart(db, item_to_change)) {
            printf("Cannot edit item %s because it exists in a cart.\n", item_to_change);
            free(merch_arr);
            return;
        }
        option_t edit_lookup = ioopm_hash_table_lookup(db->info_ht, ptr_elem(item_to_change));

        if (edit_lookup.success) {
            edit_item_menu(db, edit_lookup.element.pointer);
        }
        else puts("This item does not exist\n");
        free(merch_arr);

    } else {
        printf("Database is currently empty.\n");
    }
}

static void remove_location_info(ioopm_hash_table_t *ht, char *shelf) {
    option_t look_up = ioopm_hash_table_lookup(ht, ptr_elem(shelf));
    if (look_up.success) {
        location_info_t *location_info = look_up.element.pointer;
        free(location_info);
        option_t remove = ioopm_hash_table_remove(ht, ptr_elem(shelf));
        printf("Successfull removal: %s\n", remove.success ? "true" : "false"); //inspiration from https://stackoverflow.com/questions/17307275/what-is-the-printf-format-specifier-for-bool
    } else {
        printf("Shelf named %s doesn't appear in the database.", shelf);
    }
}

static void remove_merch_from_all_locations(ioopm_hash_table_t *location_ht, item_info_t *item_info) {
    ioopm_list_t *list_of_locations = item_info->location_list;
    size_t list_length = ioopm_linked_list_size(list_of_locations);
    for (int i = 0; i < list_length; i++) {
        option_t list_get_result = ioopm_linked_list_get(list_of_locations, 0);
        if (list_get_result.success) {
            char *shelf = list_get_result.element.pointer;
            remove_location_info(location_ht, shelf);
            ioopm_linked_list_remove(list_of_locations, 0); 
            free(shelf);
        } else {
            printf("Index pos doesn't have a value.\n");
        }
    }
    ioopm_linked_list_destroy(list_of_locations);
}

static void remove_merch_from_db(ioopm_hash_table_t *ht, item_info_t *item_info) {
    char *name = item_info->item->name;
    free(item_info->item->desc);
    free(item_info->item);
    free(item_info);
    ioopm_hash_table_remove(ht, ptr_elem(name));
    free(name);
}

static void remove_item(ioopm_db_t *db, char *name_of_merch) {
    option_t look_up = ioopm_hash_table_lookup(db->info_ht, ptr_elem(name_of_merch));
    if (look_up.success) {
        item_info_t *item_info = look_up.element.pointer;
        remove_merch_from_all_locations(db->location_ht, item_info);
        remove_merch_from_db(db->info_ht, item_info);
    } else {
        printf("Item %s doesn't appear in a info_ht, therefore cannot be removed", name_of_merch);
        return;
    }
}

void ioopm_db_remove(ioopm_db_t *db) {
    printf("Removing item...\n");
    option_t op_merch_arr = get_db_item_array(db);
    if (op_merch_arr.success) {
        size_t size = ioopm_hash_table_size(db->info_ht);
        char **merch_arr = op_merch_arr.element.pointer;
        print_list_of_merchandise(size, merch_arr);
        int index_of_merch = ask_question_int("Index of item: ");
        char *name_of_merch = merch_arr[index_of_merch - 1];
        remove_item(db, name_of_merch);
        free(merch_arr);
    } else {
        printf("\nDatabase is currently empty.\n");
    }
}

bool static accepted_int_answer(int answer) {
    if (answer == 1) {
        return true;
    }
    if (answer == 2) {
        return true;
    }
    return false;
}

static void increase_stock_on_shelf(ioopm_db_t *db, char *shelf) {
    option_t look_up = ioopm_hash_table_lookup(db->location_ht, ptr_elem(shelf));
    if (look_up.success) {
        location_info_t *location_info = look_up.element.pointer;
        int answer = abs(ask_question_int("Increase stock by: "));
        location_info->stock = location_info->stock + answer;
        printf("Increased stock on shelf %s by %d.\n", shelf, answer);
    } else {
        printf("The shelf %s doesnt exist.\n", shelf);
    }
}

static bool check_item_appear_on_shelf(item_info_t *item_info, char *shelf) {
    ioopm_list_iterator_t *iterator = ioopm_list_iterator(item_info->location_list);//iterate over list of locations
    option_t current = ioopm_iterator_current(iterator);
    do {
        char *current_shelf = current.element.pointer;
        if (strcmp(shelf, current_shelf) == 0) {
            ioopm_iterator_destroy(iterator);
            return true;
        }
        current = ioopm_iterator_next(iterator);
    } while (current.success);
    ioopm_iterator_destroy(iterator);
    return false;
}

static void add_stock_on_new_shelf(ioopm_db_t *db, char *shelf, item_info_t *item_info) {
    ioopm_list_t *list = item_info->location_list;
    ioopm_linked_list_append(list, ptr_elem(shelf));//input shelf in item's location list
    location_info_t *location_info = create_location_info(item_info);
    ioopm_hash_table_insert(db->location_ht, ptr_elem(shelf), ptr_elem(location_info));
}

static void replenish_menu(ioopm_db_t *db, char *merch_name) {
    int answer = 3;
    do {
        answer = ask_question_int("Replinishing item...\n"
                                  "[1] Increase stock on existing shelf\n"
                                  "[2] Add stock on a new shelf\n");
    } while (!accepted_int_answer(answer));
    option_t look_up = ioopm_hash_table_lookup(db->info_ht, ptr_elem(merch_name));
    if (answer == 1 && look_up.success) {
        char *shelf = ask_question_shelf("Shelf to add stock to: ");
        item_info_t *item_info = look_up.element.pointer;
        if (check_item_appear_on_shelf(item_info, shelf)) {
            increase_stock_on_shelf(db, shelf);
        } else {
            printf("This item doesn't appear on shelf %s\n", shelf);
        }
        free(shelf);
    }
    if (answer == 2 && look_up.success) {
        char *shelf = ask_question_shelf("New shelf name: ");
        option_t shelf_look_up = ioopm_hash_table_lookup(db->location_ht, ptr_elem(shelf));
        item_info_t *item_info = look_up.element.pointer;
        if (shelf_look_up.success) {
            printf("Shelf %s is already assigned to another item. Only ONE item can appear on each shelf!\n", shelf);
        } else {
            add_stock_on_new_shelf(db, shelf, item_info);
        }
    }
}

void ioopm_db_replenish(ioopm_db_t *db) {
    //Ask if user want to increase stock on already existing shelf or a new shelf
    option_t op_merch_arr = get_db_item_array(db);
    if (op_merch_arr.success) {
        char **merch_arr = op_merch_arr.element.pointer;
        size_t size = ioopm_hash_table_size(db->info_ht);
        print_list_of_merchandise(size, merch_arr);
        int index_of_merch = ask_question_int("Index of merch: ");
        char *merch = merch_arr[index_of_merch - 1];
        if (!check_item_existance(db, merch)) {
            printf("Item named %s does not exist in the db! Add it before trying to increase stock.\n", merch);
            return;
        }
        replenish_menu(db, merch);
        free(merch_arr);
    } else {
        printf("Database is currently empty.\n");
    }
}

int count_stock(ioopm_db_t *db, char *name_of_merch) {
    option_t look_up = ioopm_hash_table_lookup(db->info_ht, ptr_elem(name_of_merch));
    int stock = 0;
    if (look_up.success) {
        item_info_t *item_info = look_up.element.pointer;
        size_t length_of_location_list = ioopm_linked_list_size(item_info->location_list);
        for (int index = 0; index < length_of_location_list; index++) {
            option_t shelf_get_result = ioopm_linked_list_get(item_info->location_list, index);
            if (shelf_get_result.success) {
                char *shelf = shelf_get_result.element.pointer;
                option_t shelf_look_up = ioopm_hash_table_lookup(db->location_ht, ptr_elem(shelf));
                if (shelf_look_up.success) {
                    location_info_t *location_info = shelf_look_up.element.pointer;
                    stock += location_info->stock;
                }
            }
        }
    }
    return stock;
}

void ioopm_show_stock(ioopm_db_t *db)
{
    option_t op_merch_arr = get_db_item_array(db);
    size_t size = ioopm_hash_table_size(db->info_ht);
    if (op_merch_arr.success) {
        char **merch_arr = op_merch_arr.element.pointer;
        
        print_list_of_merchandise(size, merch_arr);
        int index_of_merch = ask_question_int("Index of merch: ");
        char *merch = merch_arr[index_of_merch - 1];
        option_t to_show_lookup = ioopm_hash_table_lookup(db->info_ht, ptr_elem(merch));
        if (to_show_lookup.success) {
            item_info_t *to_show_info = to_show_lookup.element.pointer;
            ioopm_list_iterator_t *shelf_iter = ioopm_list_iterator(to_show_info->location_list);

            size_t size = ioopm_linked_list_size(to_show_info->location_list);
            char *shelf_arr[size];
            linked_list_to_static_char_array(shelf_arr, shelf_iter);
            qsort(shelf_arr, size, sizeof(char *), string_compare);
            ioopm_iterator_destroy(shelf_iter);
            if(size == 0) {
                puts("Item is currently out of stock");
            }
            for(int i = 0; i < size; i++) {
                location_info_t *loc_info = ioopm_hash_table_lookup(db->location_ht, ptr_elem(shelf_arr[i])).element.pointer;
                int quantity = loc_info->stock;
                printf("Shelf: %s  Quantity: %d \n", shelf_arr[i], quantity);
            }
        }
        else puts("Item does not exist");
    } else {
        printf("Database is currently empty.\n");
    }
    char **to_free = op_merch_arr.element.pointer;
    if(size < 0) {
        for(int i = 0; i < size - 1; i++){
            free(to_free[i]);
        }
    }
    free(to_free);
}

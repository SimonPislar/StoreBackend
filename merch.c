#include "utils.h"
#include "merch.h"


struct item 
{
    char *name;
    char *desc;
    int price;
};

item_t *make_item(char * name, char *desc, int price) {
    item_t *item = calloc(1,sizeof(item_t)); 
    item->name = name;
    item->desc = desc;
    item->price = price;
    return item;
}

void ioopm_item_destroy(item_t *item)
{
    free(item->name);
    free(item->desc);
    free(item);
}


item_t *input_item() {
    char *name  = ask_question_string("Adding new item...\n"
                                      "Name of the item?");
    char *desc  = ask_question_string("Description? ");
    int price = -5;
    do {
        price   = ask_question_int("Price? ");
    } while (price < 1);
    item_t *input =  make_item(name, desc, price);
    return input;
     
}

void print_item(item_t *item) {

    int kr = (item->price) / 100;
    int decimals = (item->price) % 100;

    printf("Name:  %s\n", item->name);
    printf("Desc:  %s\n", item->desc);
    printf("Price: %d.%d SEK\n", kr, decimals);
}

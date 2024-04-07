#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef union {
    int int_value;
    float float_value;
    char *string_value;
} answer_t;

typedef union elem elem_t;

typedef bool(*check_func)(char *);

typedef answer_t(*convert_func)(char *);

/** @brief Checks if string consists of digits
* @param Str string to check
* @return true if string consists of digits else false
*/
bool is_number(char *str);

/**
 * @brief checks if string consists of a number bigger than 0
 * @param str string to check
 * @return true if string consists of a number bigger than 0 else false
 */
bool is_bigger_than_zero(char *str);

/** @brief Checks if string is empty
* @param Str string to check
* @return true if string is non empty else false
*/
bool not_empty(char *str);

/** @brief Gets integer from user input
* @return a valid integer
*/
int ask_question_int(char *question);

/** @brief Gets string from user input
* @return a valid string
*/
char *ask_question_string(char *question);

/** @brief Gets Shelf from user input
*   @return a valid shelf
*/
char *ask_question_shelf();

/**
 * @brief Gets single binary digit from user input
 * @param question question asked to the user
 * @return the binary digit given by the user
 */
int ask_question_binary(char *question);

/** @brief Gets string from user input
* @param a
* @param b
* @return a valid string
*/
bool ioopm_string_equiv_fn(elem_t a, elem_t b);

/**
 * @brief Hashes string to an unsigned long
 * @param str string to hash
 * @return string hashed to an unsigned long
 */
unsigned long string_knr_hash(const char *str);

/**
 * @brief prints question then reads, checks and
 * converts answer
 * @param question question to print
 * @param check function used to check answer
 * @param convert function used to convert answer
 * @return converted and valid answer
 */
answer_t ask_question(char *question, check_func check, convert_func convert);

/**
 * @brief compares two strings lexicographically using strcmp
 * @param str1 first string to be compared
 * @param str2 second string to be compared
 * @return 0 if equal, <0 if str1 is greater, >0 if str2 is greater
 */
int string_compare(const void* str1, const void* str2);

/**
 * @brief checks if answer is valid edit menu choice
 * @param answer the string to check
 * @return true if answer is a valid edit menu choice else false
 */
bool is_edit_menu_choice(char *answer);

/// @brief checks if string is in format of a shelf.
/// @param str string to check.
/// @return boolean corresponding to string beeing in shelf format.
bool is_shelf(char *str);


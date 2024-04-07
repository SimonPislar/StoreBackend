#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "utils.h"
#include "merch.h"

extern char *strdup(const char *);

typedef bool (*check_func)(char *);

typedef answer_t (*convert_func)(char *);

bool is_number(char *str) {
    int strLength = strlen(str);
    for (int i = 0; i < strLength; i++) {
        if (i == 0 && str[0] == 45 && strLength > 1) {
            continue;
        }
        if (!(isdigit(str[i]))) {
            return false;
        }
    }
    return true;
}

bool is_bigger_than_zero(char *str)
{
    if (is_number(str) && atoi(str) > 0) {
        return true;
    }
    else return false;
}

int static read_string(char *buf, int buf_size) {
    int counter = 0;
    char c = getchar();
    while (counter < buf_size && c != '\n') {
        buf[counter] = c;
        counter++;
        c = getchar();
    }
    buf[counter] = '\0';
    return counter;
}

answer_t ask_question(char *question, check_func check, convert_func convert) {
    int buf_size = 100;
    char *buf[buf_size];

    do {
        puts(question);
        read_string((char *) buf, buf_size);
    } while (!check((char *) buf));

    return convert((char *) buf);
}

bool not_empty(char *str) 
{
    return (strlen(str) > 0); 
}

bool ioopm_string_equiv_fn(elem_t a, elem_t b)
{
    char *str1 = a.pointer;
    char *str2 = b.pointer;
    return (strcmp(str1, str2) == 0);
}

unsigned long string_knr_hash(const char *str)
{
  unsigned long result = 0;
  do
    {
      result = result * 31 + *str;
    }
  while (*++str != '\0');
  return result;
}

bool is_single_binary(char *str) {
    if (!(is_number(str))) {
        return false;
    }
    int b = atoi(str);
    if (b == 0 || b == 1) {
        return true;
    }
    return false;
}


bool is_shelf(char *str) {
    if (! (strlen(str) == 3)) return false;
    if (!is_number(&str[1]) || !isalpha(str[0]) || str[0] > 90) return false;
    else return true;
}

bool is_edit_menu_choice(char *answer)
{
    char *valid_input = "1234";

    for(int i = 0; i < strlen(valid_input); i++) {
        if (answer[0] == valid_input[i]) {
            return true;
        }
    }
    return false;
}

//Inspiration from https://stackoverflow.com/questions/51129750/lexicographically-sort-in-c
int string_compare(const void *str1, const void *str2) {
    const char *s1 = str1;
    const char *s2 = str2;
    int i = 0;
    for (; tolower(s1[i]) == tolower(s2[i]); i++) {
        if (s1[i] == '\0') {
            return 0;
        }
    }
    if (s1[i] < s2[i]) {
        return 1;
    } else {
        return -1;
    }
}

int ask_question_int(char *question) {
    answer_t answer = ask_question(question, is_number, (answer_t (*)(char *)) atoi);
    return answer.int_value;
}

int ask_question_binary(char *question) {
    answer_t answer = ask_question(question, is_single_binary, (answer_t (*)(char *)) atoi);
    return answer.int_value;
}

char *ask_question_string(char *question) {
    return ask_question(question, not_empty, (answer_t (*)(char *)) strdup).string_value;
}

char *ask_question_shelf(char *question){
    char *ans = ask_question(question, is_shelf, (answer_t (*)(char *)) strdup).string_value;
    return ans;
}

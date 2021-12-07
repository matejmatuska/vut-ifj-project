#ifndef DYNAMIC_STRING_H
#define DYNAMIC_STRING_H

#define DYN_STR_LENGTH 8
#include <stdbool.h>
#include <stddef.h>
// struct of dynamic string
typedef struct
{
	char *s;
	size_t size;
	size_t buffer_size;
} dynamic_string_t;

bool dyn_str_init(dynamic_string_t *str); //initialization of dynamic string
bool dyn_str_add_character(dynamic_string_t *str, char x);	//add character to dynamic string
bool dyn_str_del_character(dynamic_string_t *str, int index);	//delete character from dynamic string
bool dyn_str_add_string(dynamic_string_t *str, char *xstr); //add string to dynamic string
bool dyn_str_compare(dynamic_string_t *str, char *xstr);	// compare two dynamic strings
void dyn_str_copy(dynamic_string_t *str1, dynamic_string_t *str2); // copy one dynamic string to another
void dyn_str_clear(dynamic_string_t *str); //erase value from dynamic string
void dyn_str_free(dynamic_string_t *str); //free dynamic string
#endif // DYNAMIC_STRING_H
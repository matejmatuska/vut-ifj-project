#ifndef DYNAMIC_STRING_H
#define DYNAMIC_STRING_H

#define DYN_STR_LENGTH 8
#include <stdbool.h>
#include <stddef.h>
typedef struct
{
	char *s;
	size_t size;
	size_t buffer_size;
} dynamic_string_t;

bool dyn_str_init(dynamic_string_t *str);
bool dyn_str_add_character(dynamic_string_t *str, char x);
bool dyn_str_del_character(dynamic_string_t *str, int index);
bool dyn_str_add_string(dynamic_string_t *str, char *xstr);
bool dyn_str_compare(dynamic_string_t *str, char *xstr);
void dyn_str_copy(dynamic_string_t *str1, dynamic_string_t *str2);
void dyn_str_clear(dynamic_string_t *str);
void dyn_str_free(dynamic_string_t *str);

#endif // DYNAMIC_STRING_H
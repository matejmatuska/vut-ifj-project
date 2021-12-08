/*
Implementace překladače imperativního jazyka IFJ21
Júlia Mazáková, xmazak02
*/
#include "dynamic_string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/**
 *Initialization of dynamic string
* @param dynamic_string_t *str -> specific dynamic string
 * @return true if success
*/
bool dyn_str_init(dynamic_string_t *str)
{
    str->s = (char *)malloc(sizeof(char));
    if (str->s == NULL)
    {
        fprintf(stderr, "Allocation error\n");
        return false;
    }
    else
    {
        str->buffer_size = DYN_STR_LENGTH;
        str->size = 0;
        str->s[str->size] = '\0';
        return true;
    }
}
/**
 *Add character to dynamic string
* @param dynamic_string_t *str -> specific string
* @param char x -> character we want to add
 * @return true if success
*/
bool dyn_str_add_character(dynamic_string_t* str, char x)
{
    str->s[str->size] = x;
    (str->size)++;
    char* tmp = str->s;
    tmp = (char*)realloc(str->s, (str->size + 1) * sizeof(char));
    if (tmp == NULL)
    {
        fprintf(stderr, "Allocation error\n");
        return false;
    }
    str->s = tmp;
    str->s[str->size] = '\0';
    return true;
}
/**
 *Delete character to dynamic string
* @param dynamic_string_t *str -> specific string
* @param char x -> character we want to delete
 * @return true if success
*/
bool dyn_str_del_character(dynamic_string_t *str, int index)
{
    for (size_t i = index; i < str->size; i++)
    {
        str->s[i] = str->s[i + 1];
        
    }
    str->size--;
    return true;
}
/**
 *Add string to dynamic string
* @param dynamic_string_t *str -> specific string
* @param char *xstr -> string we want to add
 * @return true if success
*/
bool dyn_str_add_string(dynamic_string_t *str, char *xstr)
{
    for (size_t i = 0; i < strlen(xstr); i++)
    {
        dyn_str_add_character(str, xstr[i]);
    }
    return true;
}
/**
 *Compare dynamic string with string
* @param dynamic_string_t *str -> specific dynamic string
* @param char *xstr -> string we want to compare with
 * @return true if success
*/
bool dyn_str_compare(dynamic_string_t *str, char *xstr)
{
    if (str->size == strlen(xstr))
    {
        for (size_t i = 0; i < str->size; i++)
        {
            if (str->s[i] != xstr[i])
            {
                return false;
            }
        }
        return true;
    }
    return false;
}
/**
 *Copy one dynamic string to another
* @param dynamic_string_t *str -> specific dynamic string
* @param dynamic_string_t *str2 -> dynamic string we want to copy
 * @return true if success
*/
void dyn_str_copy(dynamic_string_t *str1, dynamic_string_t *str2)
{
    for (size_t i = 0; i < str2->size; i++)
    {
        dyn_str_add_character(str1, (str2->s)[i]);
    }
    dyn_str_free(str2);
}
/**
 *Clear dynamic string
* @param dynamic_string_t *str -> specific dynamic string
 * @return true if success
*/
void dyn_str_clear(dynamic_string_t* str)
{
    if (str == NULL)
        return;

    str->size = 0;
    str->s[str->size] = 0;
    if (str->s != NULL)
    {
        free(str->s);
        str->s = NULL;
    }
}
/**
 *Free dynamic string
* @param dynamic_string_t *str -> specific dynamic string
 * @return true if success
*/
void dyn_str_free(dynamic_string_t *str)
{
    if (str == NULL) 
        return;

    if (str->s != NULL)
    {
        free(str->s);
        str->s = NULL;
    }
    
    free(str);
}

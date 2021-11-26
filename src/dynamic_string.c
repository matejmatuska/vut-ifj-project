#include "dynamic_string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

bool dyn_str_del_character(dynamic_string_t *str, int index)
{
    for (size_t i = index; i < str->size; i++)
    {
        str->s[i] = str->s[i + 1];
        
    }
    str->size--;
    return true;
}

bool dyn_str_add_string(dynamic_string_t *str, char *xstr)
{
    for (size_t i = 0; i < strlen(xstr); i++)
    {
        dyn_str_add_character(str, xstr[i]);
    }
    return true;
}

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

void dyn_str_copy(dynamic_string_t *str1, dynamic_string_t *str2)
{
    for (size_t i = 0; i < str2->size; i++)
    {
        dyn_str_add_character(str1, (str2->s)[i]);
    }
    dyn_str_free(str2);
}

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
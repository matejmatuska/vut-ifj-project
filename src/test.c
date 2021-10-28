#include "dynamic_string.h"
#include "scanner.h"
#include <stdio.h>
int func(dynamic_string_t *str)
{
    printf("%s funkcia \n",str->s);
}
int main()
{
token_t token;
token_t * tok = &token;
dynamic_string_t str1;
dynamic_string_t * str2 = &str1;
dynamic_string_t str3;
dynamic_string_t * str4 = &str3;
dyn_str_init(str2);
dyn_str_init(str4);
dyn_str_add_string(str2,"kopia");
printf("%s\n",str2->s);
printf("%s\n",str4->s);
dyn_str_copy(str4,str2);
printf("%s\n",str4->s);
printf("%s\n",str4->s);
FILE *file = fopen("test2input.txt","r");
get_source(file);
get_next_token(tok);
printf("%s\n",tok->attribute.string->s);
get_next_token(tok);
printf("%d\n",tok->attribute.integer_value);
get_next_token(tok);
printf("%d\n",tok->type);
printf("%s\n",tok->attribute.string->s);
get_next_token(tok);
printf("%d\n",tok->type);
 return 0;
}
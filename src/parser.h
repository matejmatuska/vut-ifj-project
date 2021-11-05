#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

bool program();
bool body();
bool fnc_def();
bool glob_def();
bool id_def();
bool param_list();
bool next_param();
bool ret_type_list();
bool st_list();
bool type_list();
bool next_type();
bool is_type ();
bool term_list();


#endif

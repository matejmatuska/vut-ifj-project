#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

int program();
bool body();
bool fnc_def();
bool glob_def();
bool id_def();
bool param_list();
bool next_param();
bool ret_type_list();
bool st_list();
bool st_local();
bool st_if();
bool st_while();
bool st_return();
bool st_fnc_id();
bool st_var_id();
bool option();
bool expr();
bool exp_list();
bool next_exp();
bool type_list();
bool next_type();
bool is_type ();
bool term_list();
bool next_term();
bool is_term();
bool id_list();
bool next_id();


#endif

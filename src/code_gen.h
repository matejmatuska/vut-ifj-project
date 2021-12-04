#include "dynamic_string.h"
#include <stdio.h>
#include "scanner.h"
#include "symtable.h"

typedef enum {
    NO_MATCH,  // symbol sequence on stack doesn't match any rule

    E_PLUS_E,  // E -> E + E
    E_MINUS_E, // E -> E - E
    E_MUL_E,   // E -> E* E
    E_DIV_E,   // E -> E / E
    E_INT_DIV_E, // E -> E // E

    E_EQ_E,  // E -> E == E
    E_NEQ_E, // E -> E ~= E
    E_LEQ_E, // E -> E <= E
    E_LNE_E, // E -> E < E
    E_GEQ_E, // E -> E >= E
    E_GNE_E, // E -> E > E

    LEN_E, // E -> #E
    E_CONCAT_E, // E -> E .. E

    LP_E_RP, // E -> (E)

    VAL_TO_E, // E -> i
} rule_t;

FILE* target;

void get_target(FILE* file);
void add_inst(char* inst);
void add_code(char* code);
void add_code_int(int integer);
void add_code_float(float integer);

void generate_start_of_the_func(char* func_id);
void generate_end_of_the_func(char* func_id);
void generate_call_of_the_func(char* func_id);
void generate_after_call_var_assign(int index, sym_tab_datatype from_type, char* var_id, sym_tab_datatype to_type);

void generate_newframe();
void generate_func_param_assign(char* param_id, int param_index);
void generate_default_variable_value(sym_tab_datatype type);
void generate_retval(int index, sym_tab_datatype _type);
void generate_assign_retval(int index, char* var_id);

void generate_operator(token_t* operator);
void generate_param_before_call(int index, token_t* param);
void generate_push(token_t* token);
void generate_pop(token_t* token);
void generate_declare_variable(char* var_id);
void generate_init_variable(char* var_id, sym_tab_datatype type);
void generate_operation(rule_t _rule);
void generate_program_head();
void generate_function_chr();
void generate_function_ord();
void generate_function_substr();
void generate_function_tointeger();
void generate_function_readn();
void generate_function_readi();
void generate_function_reads();
void generate_built_in_funcs();
void generate_start_of_main();
void generate_end_of_main();
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

//before generation
void code_gen_init();

 //after generation
void get_target(FILE* file);
void code_in_to_file();
void code_gen_free();

//start of the program
void generate_program_head(); //generates built in funcs etc.

//definition of the function 
void generate_start_of_the_func(char* func_id); //generates start of the function
void generate_func_param_assign(char* param_id, int param_index); // generates declaration and init of the parameters of the function
void generate_retval(int index, sym_tab_datatype type); //generates declaration of the retval
//here is the CODE of the function//
void generate_assign_retval(int index); //generates retval assign
void generate_end_of_the_func(char* func_id);// generates the end of the function
//

//
void generate_start_of_main();//generates the start of the main func
//

//pass parameter for function write
void generate_param_for_write(token_t* param);
void generate_number_of_params(int params_amount);
//

//call of the function 
void generate_newframe(); // generates newframe before passing the parametrs of the function
void generate_param_before_call(int index, token_t* param); //generates parameter pass before call

void generate_call_of_the_func(char* func_id); // calls the function
void generate_after_call_var_assign(int index, sym_tab_datatype from_type, char* var_id, sym_tab_datatype to_type); //generates the assign of the retval 
//

//
void generate_end_of_main();// generates the end of the main func
//

//expresion generation
void generate_push(token_t* token);//generates push of the value on the stack
void generate_pop(token_t* token); //generates pop of the value from the stack
void generate_declare_variable(char* var_id); //generates declaration of the variable
void generate_init_variable(char* var_id, sym_tab_datatype type);//generates init of the variable with default values
void generate_operation(rule_t rule);//generates the operation according the rule
//

//type check generation
void generate_type_check_before_asign(int index, sym_tab_datatype from_type, char* var_id, sym_tab_datatype to_type);
void generate_type_check_before_operation(char* var_id1, sym_tab_datatype type1, char* var_id2, sym_tab_datatype type2);
//

//if generation
void generate_start_of_if(int if_index);
void generate_start_of_else(int else_index, int if_index);
void generate_end_of_else(int else_index);
void generate_end_of_if(int if_index);
//

//while generation
void generate_start_of_while_head(int while_index);
void generate_start_of_while(int while_index);
void generate_end_of_while(int while_index);

//auxiliary functions
void generate_function_write();
void generate_function_chr();
void generate_function_ord();
void generate_function_substr();
void generate_function_tointeger();
void generate_function_readn();
void generate_function_readi();
void generate_function_reads();
void generate_built_in_funcs();
void generate_default_variable_value(sym_tab_datatype type);
void add_code(char* inst);
void add_code_int(int integer);
void add_code_float(float integer);
void generate_operand(token_t* operand);;
//




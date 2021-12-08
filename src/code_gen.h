/*
Implementace překladače imperativního jazyka IFJ21
Matěj Schäfer, xschaf00
*/
#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "dynamic_string.h"
#include <stdio.h>
#include "scanner.h"
#include "symtable.h"

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
//OR
void generate_retval_nil_asign(int index);//generates nil retvalue
void generate_end_of_the_func(char* func_id);// generates the end of the function
//

//pass parameter for function write
void generate_param_for_write(token_t* param);
void generate_number_of_params(int params_amount);
//

//pass parameter for general function 
void generate_newframe(); // generates newframe before passing the parametrs of the function
void generate_param_before_call(int index); //generates parameter pass before call

//call of the function and asign of the return values
void generate_call_of_the_func(char* func_id); // calls the function
void generate_after_call_var_assign(int index, sym_tab_datatype from_type, char* var_id, sym_tab_datatype to_type); //generates the assign of the retval 
//

//
void generate_start_of_program(int index);
void generate_continue_of_program(int next_index);// generates the end of the main func
void generate_end_of_program();
//

//expresion generation
void generate_push(token_t* token);//generates push of the value on the stack
void generate_pop(char* var_id); //generates pop of the value from the stack
void generate_declare_variable(char* var_id); //generates declaration of the variable
void generate_init_variable(char* var_id, sym_tab_datatype type);//generates init of the variable with default values
void generate_operation(token_type operation);//generates the operation according the rule
//

//type check generation
void generate_type_check_before_asign(sym_tab_datatype from_type, sym_tab_datatype to_type);
void generate_type_check_before_operation(sym_tab_datatype type1, sym_tab_datatype type2);
//

//
void generate_type_check_before_while_if();

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
void add_code_float(double integer);
void generate_operand(token_t* operand);
void generate_type_check_before_asign_retval(int index, sym_tab_datatype from_type, sym_tab_datatype to_type);
void generate_nil_check();
void generate_conversion_function_bf_op1();
void generate_conversion_function_bf_op2();
void generate_while_if_type_check();
//
#endif 

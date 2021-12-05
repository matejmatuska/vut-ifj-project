#include "dynamic_string.h"
#include "code_gen.h"
#include <stdio.h>
#include "scanner.h"
#include "symtable.h"

FILE* target;
dynamic_string_t code;

void get_target(FILE* file)
{
	target = file;
}

void code_gen_init()
{
	dyn_str_init(&code);
}

void code_gen_free()
{
	dyn_str_clear(&code);
}

void code_in_to_file()
{
	fprintf(target, "%s", code.s);
}

void add_code(char* inst)
{
	dyn_str_add_string(&code, inst);
}

void add_code_int(int integer)
{
	
	char tmp[40];
	sprintf(tmp, "%d", integer);
	dyn_str_add_string(&code, tmp);
	
}

void add_code_float(float integer)
{
	
	char tmp[40];
	sprintf(tmp, "%f", integer);
	dyn_str_add_string(&code, tmp);
	
}

void generate_start_of_the_func(char* func_id)
{
	add_code("#start of the function "); add_code(func_id); add_code("\n");
	add_code("LABEL "); add_code(func_id); add_code("\n");
	add_code("PUSHFRAME\n");
}

void generate_end_of_the_func(char* func_id)
{
	add_code("POPFRAME\n");
	add_code("RETURN\n");
	add_code("#end of the function "); add_code(func_id); add_code("\n");
	add_code("\n");
}

void generate_call_of_the_func(char* func_id)
{
	add_code("CALL "); add_code(func_id); add_code("\n");
}

void generate_type_check_before_asign(int index, sym_tab_datatype from_type, char* var_id, sym_tab_datatype to_type)
{
	if (from_type == INTEGER && to_type == NUMBER)
	{
		add_code("INT2FLOAT TF@retval"); add_code_int(index); add_code(" TF%retval"); add_code_int(index); add_code("\n");
	}
	else if (from_type == NUMBER && to_type == INTEGER)
	{
		add_code("FLOAT2INT TF@retval"); add_code_int(index); add_code(" TF%retval"); add_code_int(index); add_code("\n");
	}
}

void generate_type_check_before_operation(char* var_id1, sym_tab_datatype type1, char* var_id2, sym_tab_datatype type2)
{
	if (type1 == NUMBER && type2 == INTEGER)
	{
		add_code("INT2FLOAT LF@"); add_code(var_id2); add_code(" LF%"); add_code(var_id2); add_code("\n");
	}
	else if (type1 == INTEGER && type2 == NUMBER)
	{
		add_code("INT2FLOAT LF@"); add_code(var_id1); add_code(" LF%"); add_code(var_id1); add_code("\n");
	}
}

void generate_after_call_var_assign(int index, sym_tab_datatype from_type, char* var_id, sym_tab_datatype to_type)
{
	
	generate_type_check_before_asign(index, from_type, var_id, to_type);
	add_code("MOVE LF@"); add_code(var_id); add_code(" TF@retval"); add_code_int(index); add_code("\n");
}

void generate_func_param_assign(char* param_id, int param_index)
{
	add_code("DEFVAR LF@"); add_code(param_id); add_code("\n");
	add_code("MOVE LF@"); add_code(param_id); add_code(" "); add_code(" LF@%"); add_code_int(param_index); add_code("\n");
}

void generate_default_variable_value(sym_tab_datatype type)
{
	switch (type)
	{
	case STRING:
		add_code("string@");
		break;
	case INTEGER:
		add_code("int@0");
		break;
	case NUMBER:
		add_code("float@0x1.2666666666666p+0");
		break;
	case NIL:
		add_code("nil@nil");
		break;
	}
}

void generate_retval(int index, sym_tab_datatype type)
{
	add_code("DEFVAR LF@retval"); add_code_int(index); add_code("\n");
	add_code("MOVE LF@retval"); add_code_int(index); add_code(" ");

	generate_default_variable_value(type);
	add_code("\n");
}


void generate_assign_retval(int index)
{
	/*
	add_code("MOVE LF@retval"); add_code_int(index);
	*/
	add_code("POPS LF@retval"); add_code_int(index); add_code("\n");
}

/*
void generate_assing_retval_from(token_t* token)
{
	
	add_code(" LF@"); generate_operand(token); add_code("\n");
	
}
*/

dynamic_string_t* convert_string(char* string)
{
	dynamic_string_t* tmp = (dynamic_string_t*)malloc(sizeof(dynamic_string_t));
	dyn_str_init(tmp);
	int i = 0;
	while (string[i] != '\0')
	{
		if (string[i] == ' ')
		{
			dyn_str_add_string(tmp, "\\032");
		}
		else if(string[i] == '\n')
		{
			dyn_str_add_string(tmp, "\\010");
		}
		else
			dyn_str_add_character(tmp, string[i]);
		i++;
	}
	return tmp;
}

void generate_operand(token_t* operand)
{
	switch (operand->type)
	{
	case TOKEN_TYPE_ID:
		add_code(" LF@");
		add_code(operand->attribute.string->s);
		break;
	case TOKEN_TYPE_INT:
		add_code(" int@");
		add_code_int(operand->attribute.integer_value);
		break;
	case TOKEN_TYPE_STR:
		add_code(" string@");
		dynamic_string_t* tmp = convert_string(operand->attribute.string->s);
		add_code(tmp->s);
		dyn_str_free(tmp);
		break;
	case TOKEN_TYPE_DOUBLE:
		add_code(" float@0x");
		add_code_float(operand->attribute.double_value);
		add_code("p+0");
		break;
	}
}

void generate_newframe()
{
	add_code("CREATEFRAME\n");
}

void generate_param_for_write(token_t* param)
{
	add_code("PUSHS LF@"); add_code(param->attribute.string->s); add_code("\n");
}

void generate_number_of_params(int params_amount)
{
	add_code("PUSHS int@"); add_code_int(params_amount); add_code("\n");
}

void generate_param_before_call(int index, token_t* param)
{
	add_code("DEFVAR TF@%"); add_code_int(index); add_code("\n");
	add_code("MOVE "); add_code("TF@%"); add_code_int(index); generate_operand(param); add_code("\n");
}

void generate_push(token_t* token)
{
	add_code("PUSHS"); generate_operand(token); add_code("\n");
}

void generate_pop(token_t* token)
{
	add_code("POPS"); generate_operand(token); add_code("\n");
}

void generate_declare_variable(char* var_id)
{
	add_code("DEFVAR LF@"); add_code(var_id); add_code("\n");
}

void generate_init_variable(char* var_id, sym_tab_datatype type)
{
	add_code("MOVE LF@"); add_code(var_id); add_code(" "); generate_default_variable_value(type); add_code("\n");
}

void generate_function_chr()
{
	generate_start_of_the_func("chr");
	generate_retval(1, NIL);
	generate_func_param_assign("i", 1);
	generate_declare_variable("tmp");
	add_code("LT LF@tmp LF@i int@0\n");
	add_code("JUMPIFEQ return_label1 LF@tmp bool@true\n");
	add_code("GT LF@tmp LF@i int@255\n");
	add_code("JUMPIFEQ return_label1 LF@tmp bool@true\n");
	add_code("INT2CHAR LF@retval1 LF@i\n");

	add_code("LABEL return_label1\n");
	generate_end_of_the_func("chr");
}

void generate_function_ord()
{
	generate_start_of_the_func("ord");
	generate_retval(1, NIL);
	generate_func_param_assign("s", 1);
	generate_func_param_assign("i", 2);
	generate_declare_variable("length");
	generate_declare_variable("tmp");

	add_code("STRLEN LF@length LF@s\n");
	add_code("GT LF@tmp LF@i LF@length\n");
	add_code("JUMPIFEQ return_label2 LF@tmp bool@true\n");

	add_code("SUB LF@i LF@i int@1\n");
	add_code("STRI2INT LF@retval1 LF@s LF@i\n");

	add_code("LABEL return_label2\n");
	generate_end_of_the_func("ord");
}


void generate_function_substr()
{
	generate_start_of_the_func("substr");
	generate_retval(1, STRING);
	generate_func_param_assign("s", 1);
	generate_func_param_assign("i", 2);
	generate_func_param_assign("j", 3);
	generate_declare_variable("length");
	generate_declare_variable("tmp");
	generate_declare_variable("sign");
	/*
	add_code("TYPE tmp LF@s\n");
	add_code("JUMPIFEQ exit8_label LF@tmp string@nil\n");
	add_code("TYPE tmp LF@i\n");
	add_code("JUMPIFEQ exit8_label LF@tmp string@nil\n");
	add_code("TYPE tmp LF@j\n");
	add_code("JUMPIFEQ exit8_label LF@tmp string@nil\n");
	*/

	add_code("STRLEN LF@length LF@s\n");
	add_code("GT LF@tmp LF@i LF@length\n");
	add_code("JUMPIFEQ return_label3 LF@tmp bool@true\n");
	add_code("GT LF@tmp LF@j LF@length\n");
	add_code("JUMPIFEQ return_label3 LF@tmp bool@true\n");
	add_code("GT LF@tmp LF@i LF@j\n");
	add_code("JUMPIFEQ return_label3 LF@tmp bool@true\n");
	
	add_code("SUB LF@i LF@i int@1\n");
	add_code("SUB LF@j LF@j int@1\n");

	add_code("LABEL loop\n");
	add_code("GETCHAR LF@sign LF@s LF@i\n");
	add_code("CONCAT LF@retval1 LF@retval1 LF@sign\n");

	add_code("ADD LF@i LF@i int@1\n");
	add_code("GT LF@tmp LF@i LF@j\n");
	add_code("JUMPIFEQ return_label3 LF@tmp bool@true\n");
	add_code("JUMP loop\n");
	/*
	add_code("LABEL exit8_label\n");
	add_code("EXIT 8\n");
	*/
	add_code("LABEL return_label3\n");
	generate_end_of_the_func("substr");
}

void generate_function_tointeger()
{
	generate_start_of_the_func("tointeger");
	generate_retval(1, NIL);
	generate_func_param_assign("f", 1);
	add_code("FLOAT2INT LF@retval1 LF@f\n");
	generate_end_of_the_func("tointeger");
}

void generate_function_write()
{
	add_code("LABEL write\n");
	add_code("POPS GF@tmp1\n");
	add_code("LABEL loop2\n");
	add_code("GT GF@tmp2 GF@tmp1 int@0\n");
	add_code("JUMPIFEQ end_loop2 GF@tmp2 bool@false \n");
	add_code("POPS GF@tmp2\n");
	add_code("SUB GF@tmp1 GF@tmp1 int@1\n");
	add_code("WRITE GF@tmp2\n");
	add_code("JUMP loop2\n");
	add_code("LABEL end_loop2\n");

	generate_end_of_the_func("write");
}

void generate_function_readn()
{
	generate_start_of_the_func("readn");
	generate_retval(1, NUMBER);
	add_code("READ LF@retval1 float\n");
	generate_end_of_the_func("readn");
}

void generate_function_readi()
{
	generate_start_of_the_func("readi");
	generate_retval(1, INTEGER);
	add_code("READ LF@retval1 int\n");
	generate_end_of_the_func("readi");
}

void generate_function_reads()
{
	generate_start_of_the_func("reads");
	generate_retval(1, STRING);
	add_code("READ LF@retval1 string\n");
	generate_end_of_the_func("reads");
}

void generate_built_in_funcs()
{
	generate_function_write();
	generate_function_reads();
	generate_function_readi();
	generate_function_readn();
	generate_function_tointeger();
	generate_function_substr();
	generate_function_ord();
	generate_function_chr();

}

void generate_program_head()
{
	add_code(".IFJcode21\n");
	add_code("DEFVAR GF@tmp1\n");
	add_code("DEFVAR GF@tmp2\n");
	add_code("JUMP main\n");
	add_code("\n");
	generate_built_in_funcs();
}

void generate_start_of_main()
{
	add_code("LABEL main\n");
	add_code("CREATEFRAME\n");
	add_code("PUSHFRAME\n");
}

void generate_end_of_main()
{
	add_code("CLEARS\n");
	add_code("EXIT int@0\n");
	add_code("#end of main\n");
}

void generate_start_of_while_head(int while_index)
{
	add_code("LABEL while"); add_code_int(while_index); add_code("\n");
}

void generate_start_of_while(int while_index)
{
	add_code("POPS GF@tmp1\n");
	add_code("JUMPIFEQ end_while"); add_code_int(while_index); add_code(" GF@tmp1 bool@false\n");
}

void generate_end_of_while(int while_index)
{
	add_code("LABEL end_while"); add_code_int(while_index); add_code("\n");
}

void generate_start_of_if(int if_index)
{
	add_code("POPS GF@tmp1\n");
	add_code("JUMPIFEQ end_if"); add_code_int(if_index); add_code(" GF@tmp1 bool@false\n");

}

void generate_start_of_else(int else_index, int if_index)
{
	add_code("JUMP end_else"); add_code_int(else_index); add_code("\n");
	add_code("LABEL end_if"); add_code_int(if_index); add_code("\n");
}

void generate_end_of_else(int else_index)
{
	add_code("LABEL end_else"); add_code_int(else_index); add_code("\n");
}

void generate_end_of_if(int if_index)
{
	add_code("LABEL end_if"); add_code_int(if_index); add_code("\n");
}

void generate_operation(rule_t rule)
{
	switch (rule)
	{
	case E_PLUS_E:
		add_code("ADS ");
		break;
	case E_MINUS_E:
		add_code("SUBS ");
		break;
	case E_MUL_E:
		add_code("MULS");
		break;
	case E_DIV_E:
		add_code("DIV");
		break;
	case E_INT_DIV_E:
		add_code("IDIV");
		break;
	case E_EQ_E:
		add_code("EQS");
		break;
	case E_NEQ_E:
		add_code("EQS"); add_code("\n");
		add_code("NOTS");
		break;
	case E_LEQ_E:
		add_code("POPS GF@tmp1\n");
		add_code("POPS GF@tmp2\n");
		add_code("PUSHS GF@tmp2\n");
		add_code("PUSHS GF@tmp1\n");
		add_code("EQS\n");
		add_code("PUSHS GF@tmp2\n");
		add_code("PUSHS GF@tmp1\n");
		add_code("LTS\n");
		add_code("ORS\n");
		break;
	case E_LNE_E:
		add_code("LTS");
		break;
	case E_GEQ_E:
		add_code("POPS GF@tmp1\n");
		add_code("POPS GF@tmp2\n");
		add_code("PUSHS GF@tmp2\n");
		add_code("PUSHS GF@tmp1\n");
		add_code("EQS\n");
		add_code("PUSHS GF@tmp2\n");
		add_code("PUSHS GF@tmp1\n");
		add_code("GTS\n");
		add_code("ORS\n");
		break;

	case E_GNE_E:
		add_code("GTS");
		break;
	case LEN_E:
		add_code("POPS GF@tmp1\n");
		add_code("STRLEN GF@tmp1 GF@tmp1\n");
		add_code("PUSHS  GF@tmp1\n");
		break;
	case E_CONCAT_E:
		add_code("POPS GF@tmp1\n");
		add_code("POPS GF@tmp2\n");
		add_code("CONCAT GF@tmp1 GF@tmp2 GF@tmp1\n");
		add_code("PUSHS GF@tmp1\n");
		break;
	}
	add_code("\n");
}
	








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

void add_code_float(double integer)
{
	char tmp[40];
	sprintf(tmp, "%a", integer);
	dyn_str_add_string(&code, tmp);
}

void add_code_index(int integer)
{
	char tmp[40];
	sprintf(tmp, "?%d", integer);
	dyn_str_add_string(&code, tmp);
}

void add_id(char* var_id, size_t level, size_t UID)
{
	add_code(var_id); 
	add_code("-");
	
	char tmp[40];
	sprintf(tmp, "%ld", level);
	dyn_str_add_string(&code, tmp);

	add_code("-"); 

	sprintf(tmp, "%ld", UID);
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

void generate_type_check_before_asign(sym_tab_datatype from_type, sym_tab_datatype to_type)
{
	
	if (from_type == INTEGER && to_type == NUMBER)
	{
		add_code("INT2FLOATS\n");//converts from_type to FLOAT
	}
	
	/*
	if (to_type == NUMBER)
	{
		add_code("POPS GF@tmp1\n");
		add_code("TYPE GF@tmp2 GF@tmp1\n");
		add_code("JUMPIFEQ __conversion__ GF@tmp2 string@int\n");
		
	}
	*/
}

void generate_type_check_before_asign_retval(int index, sym_tab_datatype from_type, sym_tab_datatype to_type)
{
	if (from_type == INTEGER && to_type == NUMBER)
	{
		add_code("INT2FLOAT TF@retval"); add_code_index(index); add_code(" TF%retval"); add_code_index(index); add_code("\n");
	}
	else if (from_type == NUMBER && to_type == INTEGER)
	{
		add_code("FLOAT2INT TF@retval"); add_code_index(index); add_code(" TF%retval"); add_code_index(index); add_code("\n");
	}
}

void generate_type_check_before_operation(sym_tab_datatype type1, sym_tab_datatype type2)
{
	if (type1 == NUMBER && type2 == INTEGER)
	{
		add_code("CALL !conversion_func_bf_op1\n");
	}
	else if (type1 == INTEGER && type2 == NUMBER)
	{
		add_code("CALL !conversion_func_bf_op2\n");
	}
}

void generate_after_call_var_assign(int index, sym_tab_datatype from_type, char* var_id, size_t level, size_t UID, sym_tab_datatype to_type)
{
	
	generate_type_check_before_asign_retval(index, from_type, to_type);
	add_code("MOVE LF@"); add_id(var_id, level, UID); add_code(" TF@retval"); add_code_index(index); add_code("\n");
}

void generate_func_param_assign(char* param_id, size_t level, size_t UID,int param_index)
{
	add_code("DEFVAR LF@"); add_id(param_id,level,UID); add_code("\n");
	add_code("MOVE LF@"); add_id(param_id, level, UID); 
	add_code(" LF@%"); add_code(param_index); add_code("\n");
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
	default:
		break;
	}
}

void generate_retval(int index, sym_tab_datatype type)
{
	add_code("DEFVAR LF@retval"); add_code_index(index); add_code("\n");
	add_code("MOVE LF@retval"); add_code_index(index); add_code(" ");

	generate_default_variable_value(type);
	add_code("\n");
}


void generate_assign_retval(int index)
{
	add_code("POPS LF@retval"); add_code_index(index); add_code("\n");
}

void generate_retval_nil_asign(int index)
{
	add_code("MOVE LF@retval"); add_code_index(index); add_code(" nil@nil\n");
}

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
        else if(string[i] == '\t')
        {
            dyn_str_add_string(tmp, "\\009");
        }
        else if(string[i] == '\\')
        {
            dyn_str_add_string(tmp, "\\092");
        }
        else
            dyn_str_add_character(tmp, string[i]);
        i++;
    }
    return tmp;
}

void generate_operand(token_t* operand, size_t level, size_t UID)
{
	switch (operand->type)
	{
	case TOKEN_TYPE_ID:
		add_code(" LF@");
		add_id(operand->attribute.string->s, level, UID);
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
		add_code(" float@");
		add_code_float(operand->attribute.double_value);
		break;
	case TOKEN_TYPE_EXP:
		add_code(" float@");
		add_code_float(operand->attribute.double_value);
		break;

	case TOKEN_TYPE_SIGN_EXP:
		add_code(" float@");
		add_code_float(operand->attribute.double_value);
		break;

	case TOKEN_TYPE_KW:
		if (operand->attribute.keyword == KW_NIL)
			add_code(" nil@nil");
		break;
	default:
		break;
	}
}

void generate_newframe()
{
	add_code("CREATEFRAME\n");
}

void generate_param_for_write(token_t* param,size_t level, size_t UID)
{
	add_code("PUSHS"); generate_operand(param, level, UID); add_code("\n");
}

void generate_number_of_params(int params_amount)
{
	add_code("PUSHS int@"); add_code_int(params_amount); add_code("\n");
}

void generate_param_before_call(int index)
{
	add_code("DEFVAR TF@%"); add_code_int(index); add_code("\n");
	add_code("POPS TF@%"); add_code_int(index); add_code("\n");
}

void generate_push(token_t* token, size_t level, size_t UID)
{
	add_code("PUSHS"); generate_operand(token, level, UID); add_code("\n");
}

void generate_pop(char* var_id, size_t level, size_t UID)
{
	add_code("POPS LF@"); add_id(var_id, level, UID); add_code("\n");
}

void generate_declare_variable(char* var_id, size_t level, size_t UID)
{
	add_code("DEFVAR LF@"); add_id(var_id, level, UID); add_code("\n");
	add_code("MOVE LF@"); add_id(var_id, level, UID); add_code(" nil@nil"); add_code("\n");
}

void generate_init_variable(char* var_id, size_t level, size_t UID, sym_tab_datatype type)
{
	add_code("MOVE LF@"); add_id(var_id, level, UID); add_code(" "); generate_default_variable_value(type); add_code("\n");
}

void generate_while_if_type_check()
{
	add_code("#start of the function type_check_before_while_if\n");
	add_code("LABEL !type_check_before_while_if\n");
	add_code("POPS GF@tmp?1\n");
	add_code("TYPE GF@tmp?2 GF@tmp?1\n");
	add_code("JUMPIFEQ !skip_4 GF@tmp?2 string@bool \n");
	add_code("JUMPIFEQ !skip_5 GF@tmp?2 string@nil\n");
	add_code("PUSHS bool@true\n");
	add_code("RETURN\n");
	add_code("LABEL !skip_5\n");
	add_code("PUSHS bool@false\n");
	add_code("RETURN\n");
	add_code("LABEL !skip_4\n");
	add_code("PUSHS GF@tmp?1\n");
	add_code("RETURN\n");
	add_code("#end of the function type_check_before_while_if\n");
	add_code("\n");
}

void generate_conversion_function_bf_op1()
{
	add_code("#start of the function conversion_bf_op1\n");
	add_code("LABEL !conversion_func_bf_op1\n");
	add_code("POPS GF@tmp?1\n");
	add_code("TYPE GF@tmp?3 GF@tmp?1\n");
	add_code("JUMPIFEQ !skip_2 GF@tmp?3 string@nil\n");
	add_code("PUSHS GF@tmp?1\n");
	add_code("INT2FLOATS\n");
	add_code("RETURN\n");
	add_code("LABEL !skip_2\n");
	add_code("PUSHS GF@tmp?1\n");
	add_code("RETURN\n");
	add_code("#end of the function conversion_bf_op1\n");
	add_code("\n");
}

void generate_conversion_function_bf_op2()
{
	add_code("#start of the function conversion_bf_op2\n");
	add_code("LABEL !conversion_func_bf_op2\n");
	add_code("POPS GF@tmp?2\n");

	add_code("POPS GF@tmp?1\n");
	add_code("TYPE GF@tmp?3 GF@tmp?1\n");
	add_code("JUMPIFEQ !skip_3 GF@tmp?3 string@nil\n");
	add_code("PUSHS GF@tmp?1\n");
	add_code("INT2FLOATS\n");
	add_code("PUSHS GF@tmp?2\n");
	add_code("RETURN\n");
	add_code("LABEL !skip_3\n");
	add_code("PUSHS GF@tmp?1\n");
	add_code("PUSHS GF@tmp?2\n");
	add_code("RETURN\n");
	add_code("#end of the function conversion_bf_op2\n");
	add_code("\n");

}

void generate_function_chr()
{
	generate_start_of_the_func("chr");
	generate_retval(1, NIL);
	//generate_func_param_assign("i", 1);
	add_code("DEFVAR LF@i\n");
	add_code("MOVE LF@i LF@%1\n");
	//generate_declare_variable("tmp");
	add_code("DEFVAR LF@tmp\n");
	add_code("MOVE LF@tmp nil@nil\n");

	add_code("TYPE LF@tmp LF@i\n");
	add_code("JUMPIFEQ !error_label LF@tmp string@nil\n");
	add_code("LT LF@tmp LF@i int@0\n");
	add_code("JUMPIFEQ !nil_return LF@tmp bool@true\n");
	add_code("GT LF@tmp LF@i int@255\n");
	add_code("JUMPIFEQ !nil_return LF@tmp bool@true\n");

	add_code("INT2CHAR "); add_code("LF@retval"); add_code_index(1); add_code(" LF@i\n");
	add_code("LABEL !nil_return\n");
	generate_end_of_the_func("chr");
}

void generate_function_ord()
{
	generate_start_of_the_func("ord");
	generate_retval(1, NIL);
	//generate_func_param_assign("s", 1);
	add_code("DEFVAR LF@s\n");
	add_code("MOVE LF@s LF@%1\n");
	//generate_func_param_assign("i", 2);
	add_code("DEFVAR LF@i\n");
	add_code("MOVE LF@i LF@%2\n");
	//generate_declare_variable("length");
	add_code("DEFVAR LF@length\n");
	add_code("MOVE LF@length nil@nil\n");
	//generate_declare_variable("tmp");
	add_code("DEFVAR LF@tmp\n");
	add_code("MOVE LF@tmp nil@nil\n");

	add_code("TYPE LF@tmp LF@s\n");
	add_code("JUMPIFEQ !error_label LF@tmp string@nil\n");
	add_code("TYPE LF@tmp LF@i\n");
	add_code("JUMPIFEQ !error_label LF@tmp string@nil\n");

	add_code("STRLEN LF@length LF@s\n");
	add_code("GT LF@tmp LF@i LF@length\n");
	add_code("JUMPIFEQ !nil_return2 LF@tmp bool@true\n");
	add_code("LT LF@tmp LF@i int@1\n");
	add_code("JUMPIFEQ !nil_return2 LF@tmp bool@true\n");

	add_code("SUB LF@i LF@i int@1\n");

	add_code("STRI2INT "); add_code("LF@retval");
    add_code_index(1); add_code(" LF@s LF@i\n");

	add_code("LABEL !nil_return2\n");
	generate_end_of_the_func("ord");
}


void generate_function_substr()
{
	generate_start_of_the_func("substr");
	generate_retval(1, STRING);
	//generate_func_param_assign("s", 1);
	add_code("DEFVAR LF@s\n");
	add_code("MOVE LF@s LF@%1\n");
	//generate_func_param_assign("i", 2);
	add_code("DEFVAR LF@i\n");
	add_code("MOVE LF@i LF@%2\n");
	//generate_func_param_assign("j", 3);
	add_code("DEFVAR LF@j\n");
	add_code("MOVE LF@j LF@%3\n");
	//generate_declare_variable("length");
	add_code("DEFVAR LF@length\n");
	add_code("MOVE LF@length nil@nil\n");
	//generate_declare_variable("tmp");
	add_code("DEFVAR LF@tmp\n");
	add_code("MOVE LF@tmp nil@nil\n");
	//generate_declare_variable("sign");
	add_code("DEFVAR LF@sign\n");
	add_code("MOVE LF@sign nil@nil\n");
	
	add_code("TYPE LF@tmp LF@s\n");
	add_code("JUMPIFEQ !error_label LF@tmp string@nil\n");
	add_code("TYPE LF@tmp LF@i\n");
	add_code("JUMPIFEQ !error_label LF@tmp string@nil\n");
	add_code("TYPE LF@tmp LF@j\n");
	add_code("JUMPIFEQ !error_label LF@tmp string@nil\n");

	add_code("STRLEN LF@length LF@s\n");
	add_code("GT LF@tmp LF@i LF@length\n");
	add_code("JUMPIFEQ !return_label3 LF@tmp bool@true\n");
	add_code("LT LF@tmp LF@i int@1\n");
	add_code("JUMPIFEQ !return_label3 LF@tmp bool@true\n");

	add_code("GT LF@tmp LF@j LF@length\n");
	add_code("JUMPIFEQ !return_label3 LF@tmp bool@true\n");
	add_code("LT LF@tmp LF@j int@1\n");
	add_code("JUMPIFEQ !return_label3 LF@tmp bool@true\n");

	add_code("GT LF@tmp LF@i LF@j\n");
	add_code("JUMPIFEQ !return_label3 LF@tmp bool@true\n");
	
	add_code("SUB LF@i LF@i int@1\n");
	add_code("SUB LF@j LF@j int@1\n");

	add_code("LABEL !loop\n");
	add_code("GETCHAR LF@sign LF@s LF@i\n");
	add_code("CONCAT "); add_code("LF@retval"); add_code_index(1);
    add_code(" LF@retval"); add_code_index(1); add_code(" LF@sign\n");

	add_code("ADD LF@i LF@i int@1\n");
	add_code("GT LF@tmp LF@i LF@j\n");
	add_code("JUMPIFEQ !return_label3 LF@tmp bool@true\n");
	add_code("JUMP !loop\n");
	
	add_code("LABEL !return_label3\n");
	generate_end_of_the_func("substr");
}

void generate_function_tointeger()
{
	generate_start_of_the_func("tointeger");
	generate_retval(1, NIL);
	//generate_func_param_assign("f", 1);
	add_code("DEFVAR LF@f\n");
	add_code("MOVE LF@f LF@%1\n");

	add_code("TYPE GF@tmp?1 LF@f\n");
	add_code("JUMPIFEQ !return_label4 GF@tmp?1 string@nil\n");

	add_code("FLOAT2INT "); add_code("LF@retval");
    add_code_index(1); add_code(" LF@f\n");
	add_code("LABEL !return_label4\n");
	generate_end_of_the_func("tointeger");
}

void generate_function_write()
{	
	add_code("#start of the function write\n");
	add_code("LABEL write\n");
	add_code("POPS GF@tmp?1\n");
	add_code("LABEL !loop2\n");
	add_code("GT GF@tmp?2 GF@tmp?1 int@0\n");
	add_code("JUMPIFEQ !end_loop2 GF@tmp?2 bool@false \n");
	add_code("POPS GF@tmp?2\n");
	add_code("SUB GF@tmp?1 GF@tmp?1 int@1\n");
	add_code("JUMPIFNEQ !__skip__ GF@tmp?2 nil@nil \n");
	add_code("WRITE string@nil\n");
	add_code("LABEL !__skip__\n");
	add_code("WRITE GF@tmp?2\n");
	add_code("JUMP !loop2\n");
	add_code("LABEL !end_loop2\n");
	add_code("RETURN\n");
	add_code("#end of the function write\n");
	add_code("\n");
}

void generate_function_readn()
{
	generate_start_of_the_func("readn");
	generate_retval(1, NUMBER);

	add_code("READ "); add_code("LF@retval"); add_code_index(1); add_code(" float\n");

	generate_end_of_the_func("readn");
}

void generate_function_readi()
{
	generate_start_of_the_func("readi");
	generate_retval(1, INTEGER);

	add_code("READ "); add_code("LF@retval"); add_code_index(1); add_code(" int\n");

	generate_end_of_the_func("readi");
}

void generate_function_reads()
{
	generate_start_of_the_func("reads");
	generate_retval(1, STRING);

	add_code("READ "); add_code("LF@retval"); add_code_index(1); add_code(" string\n");

	generate_end_of_the_func("reads");
}

void generate_built_in_funcs()
{
	generate_conversion_function_bf_op1();
	generate_conversion_function_bf_op2();
	generate_while_if_type_check();
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
	add_code("DEFVAR GF@tmp?1\n");
	add_code("DEFVAR GF@tmp?2\n");
	add_code("DEFVAR GF@tmp?3\n");
	add_code("JUMP !MAIN"); add_code_index(1); add_code("\n");
	add_code("\n");
	add_code("LABEL !error_label\n");
	add_code("CLEARS\n");
	add_code("EXIT int@8\n");
	add_code("\n");
	generate_built_in_funcs();
}

void generate_start_of_program(int index)
{
	add_code("LABEL !MAIN"); add_code_index(index); add_code("\n");
	add_code("CREATEFRAME\n");
	add_code("PUSHFRAME\n");
	
}

void generate_continue_of_program(int next_index)
{
	add_code("POPFRAME\n");
	add_code("JUMP !MAIN"); add_code_index(next_index + 1); add_code("\n");
}

void generate_end_of_program()
{
	add_code("POPFRAME\n");
	add_code("CLEARS\n");
	add_code("EXIT int@0\n");
}

void generate_type_check_before_while_if()
{
	add_code("CALL !type_check_before_while_if\n");
}

void generate_start_of_while_head(int while_index)
{
	add_code("LABEL while"); add_code_index(while_index); add_code("\n");
}

void generate_start_of_while(int while_index)
{
	add_code("POPS GF@tmp?1\n");
	add_code("JUMPIFEQ end_while"); add_code_index(while_index); add_code(" GF@tmp?1 bool@false\n");
}

void generate_end_of_while(int while_index)
{
	add_code("JUMP while"); add_code_index(while_index); add_code("\n");
	add_code("LABEL end_while"); add_code_index(while_index); add_code("\n");
}

void generate_start_of_if(int if_index)
{
	add_code("POPS GF@tmp?1\n");
	add_code("JUMPIFEQ end_if"); add_code_index(if_index); add_code(" GF@tmp?1 bool@false\n");

}

void generate_start_of_else(int else_index, int if_index)
{
	add_code("JUMP end_else"); add_code_index(else_index); add_code("\n");
	add_code("LABEL end_if"); add_code_index(if_index); add_code("\n");
}

void generate_end_of_else(int else_index)
{
	add_code("LABEL end_else"); add_code_index(else_index); add_code("\n");
}

void generate_end_of_if(int if_index)
{
	add_code("LABEL end_if"); add_code_index(if_index); add_code("\n");
}

void generate_nil_check()
{
	add_code("POPS GF@tmp?1\n");
	add_code("TYPE GF@tmp?3 GF@tmp?1\n");
	add_code("JUMPIFEQ !error_label GF@tmp?3 string@nil\n");
	add_code("POPS GF@tmp?2\n");
	add_code("TYPE GF@tmp?3 GF@tmp?2\n");
	add_code("JUMPIFEQ !error_label GF@tmp?3 string@nil\n");
	add_code("PUSHS GF@tmp?2\n");
	add_code("PUSHS GF@tmp?1\n");
}

void generate_operation(token_type operation)
{
	switch (operation)
	{
	case TOKEN_TYPE_PLUS:
		generate_nil_check();

		add_code("ADDS\n");
		break;
	case TOKEN_TYPE_MINUS:
		generate_nil_check();

		add_code("SUBS\n");
		break;
	case TOKEN_TYPE_MUL_SIGN:
		generate_nil_check();

		add_code("MULS\n");
		break;
	case TOKEN_TYPE_DIV_SIGN:
		generate_nil_check();

		add_code("POPS GF@tmp?1\n");
		add_code("JUMPIFNEQ !next_label GF@tmp?1 float@0x0p+0\n");
		add_code("CLEARS \n");
		add_code("EXIT int@9\n");
		add_code("LABEL !next_label\n");
		add_code("PUSHS GF@tmp?1\n");
		add_code("DIVS\n");
		break;
	case TOKEN_TYPE_WN_DIV_SIGN:
		generate_nil_check();

		add_code("POPS GF@tmp?1\n");
		add_code("JUMPIFNEQ !next_label GF@tmp?1 int@0\n");
		add_code("CLEARS \n");
		add_code("EXIT int@9\n");
		add_code("LABEL !next_label\n");
		add_code("INT2FLOATS\n");
		add_code("PUSHS GF@tmp?1\n");
		add_code("INT2FLOATS\n");
		add_code("DIVS\n");
		add_code("FLOAT2INTS\n");
		break;
	case TOKEN_TYPE_COMPARING:

		add_code("EQS\n");
		break;
	case TOKEN_TYPE_COMPARING2:

		add_code("EQS\n"); add_code("\n");
		add_code("NOTS\n");
		break;
	case TOKEN_TYPE_LESSEQ:
		generate_nil_check();

		add_code("POPS GF@tmp?1\n");
		add_code("POPS GF@tmp?2\n");
		add_code("PUSHS GF@tmp?2\n");
		add_code("PUSHS GF@tmp?1\n");
		add_code("EQS\n");
		add_code("PUSHS GF@tmp?2\n");
		add_code("PUSHS GF@tmp?1\n");
		add_code("LTS\n");
		add_code("ORS\n");
		break;
	case TOKEN_TYPE_LESS:
		generate_nil_check();

		add_code("LTS");
		break;
	case TOKEN_TYPE_GREATEREQ:
		generate_nil_check();

		add_code("POPS GF@tmp?1\n");
		add_code("POPS GF@tmp?2\n");
		add_code("PUSHS GF@tmp?2\n");
		add_code("PUSHS GF@tmp?1\n");
		add_code("EQS\n");
		add_code("PUSHS GF@tmp?2\n");
		add_code("PUSHS GF@tmp?1\n");
		add_code("GTS\n");
		add_code("ORS\n");
		break;

	case TOKEN_TYPE_GREATER:
		generate_nil_check();

		add_code("GTS\n");
		break;
	case TOKEN_TYPE_LENGTH:
		
		add_code("POPS GF@tmp?1\n");
		add_code("TYPE GF@tmp?3 GF@tmp?1\n");
		add_code("JUMPIFEQ !error_label GF@tmp?3 string@nil\n");
		add_code("PUSHS GF@tmp?1\n");
		
		add_code("POPS GF@tmp?1\n");
		add_code("STRLEN GF@tmp?1 GF@tmp?1\n");
		add_code("PUSHS  GF@tmp?1\n");
		break;
	case TOKEN_TYPE_DOUBLE_DOT:
		generate_nil_check();

		add_code("POPS GF@tmp?1\n");
		add_code("POPS GF@tmp?2\n");
		add_code("CONCAT GF@tmp?1 GF@tmp?2 GF@tmp?1\n");
		add_code("PUSHS GF@tmp?1\n");
		break;
	default:
		break;
	}
	add_code("\n");
}
	








#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "parser.h"
#include "expr_parser.h"

#include "scanner.h"
#include "dynamic_string.h"
#include "symtable.h"
#include "ST_stack.h"

#include "error.h"

#define TOK_IS_KW(kw) \
    (token->type == TOKEN_TYPE_KW && token->attribute.keyword == (kw))

#define TOK_IS_ID \
    (token->type == TOKEN_TYPE_ID)

#define  TOK_IS_TYPE(_type) \
    (token->type == (_type))

#define TOK_IS_OP() \
    (token->type == TOKEN_TYPE_MINUS || token->type == TOKEN_TYPE_PLUS || token->type == TOKEN_TYPE_DIV_SIGN || token->type == TOKEN_TYPE_MUL_SIGN )

#define SYM_FIND() \
    (sym_tab_find_in_table(top_table(scope), token->attribute.string->s))

#define ID_NAME() \
    (token->attribute.string->s)


token_t *token;

#define GET_NEXT_TOKEN()                            \
    do {                                            \
        if (get_next_token(token) == LEX_ERR) {     \
            ERROR = LEX_ERR;                        \
            return false;                           \
        }                                           \
    } while (0)


int ERROR = 0;

ST_stack* scope = NULL;

sym_tab_datatype get_datatype();

int program();
bool body();
bool fnc_def();
bool glob_def();
bool id_def();
bool param_list(data_type *par_type, int *num);
bool next_param(data_type *par_type, int *num);
bool ret_type_list(data_type *ret_type, int * num);
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
bool type_list(data_type *types, int * num);
bool next_type(data_type *types, int * num);
bool is_type ();
bool term_list();
bool next_term();
bool is_term();
bool id_list();
bool next_id();
bool is_type_data();

int parse()
{
    scope = init_ST_stack();
    token = malloc(sizeof(token_t));

    int result = program();

    free(token);
    free_ST_stack(&scope);
    return result;
}

// <prog> -> require STRING <body> EOF
int program() {

    GET_NEXT_TOKEN();

    if (!TOK_IS_KW(KW_REQUIRE)) {
        ERROR = SYNTAX_ERR;
        return ERROR;
    }

    GET_NEXT_TOKEN();
    if (TOK_IS_TYPE(TOKEN_TYPE_STR)
        && !dyn_str_compare(token->attribute.string, "ifj21")) {
        ERROR = SYNTAX_ERR;
        return ERROR;
    }

    if (!body())
        return ERROR;

    GET_NEXT_TOKEN();
    if (token->type != TOKEN_TYPE_EOF) {
        ERROR = SYNTAX_ERR;
    };

    return ERROR;
}

bool body() {
    push(&scope);
    if(TOK_IS_KW(KW_GLOBAL) || TOK_IS_KW(KW_FUNCTION) || TOK_IS_TYPE(TOKEN_TYPE_EOF) || TOK_IS_ID){

    } else
        GET_NEXT_TOKEN();
    if (TOK_IS_KW(KW_GLOBAL)) {
        return glob_def();
    } else if (TOK_IS_KW(KW_FUNCTION)) {
        return fnc_def();
    } else if (token->type == TOKEN_TYPE_ID) {
        return id_def();
    } else if (token->type == TOKEN_TYPE_EOF) {
        return true;
    }
    return false;
}
//function (...)
bool fnc_def() {
    GET_NEXT_TOKEN();
    sym_tab_item_t *item;
    data_type ret_type = NULL;
    data_type par_type = NULL;
    int par_num = 0;
    int ret_num = 0;
    if (!TOK_IS_ID) {
        return false;
    } else {
        item = SYM_FIND();
        if(item == NULL){
            item = sym_tab_add_item(top_table(scope), ID_NAME());
            sym_tab_add_data_function(item, ret_type, par_type, true, true, par_num);

            //todo add item into scope
        }
    }

    GET_NEXT_TOKEN();
    if (!TOK_IS_TYPE(TOKEN_TYPE_LEFTB)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    //create_data_type(par_type);
    if (!param_list(&par_type, &par_num)) {
        return false;
    }

    sym_tab_add_data_function(item, ret_type, par_type, true, true, par_num);

//    GET_NEXT_TOKEN();
    if (!TOK_IS_TYPE(TOKEN_TYPE_RIGHTB)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    //create_data_type(ret_type);
    if (!ret_type_list(&ret_type, &ret_num)) {
        return false;
    }

    sym_tab_add_data_function(item, ret_type, par_type, true, true, par_num);

    if (!st_list()) {
        return false;
    }
    if (!TOK_IS_KW(KW_END)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    if (!body())
        return false;
    return true;
}

bool glob_def() {

    sym_tab_item_t *item = NULL;
    data_type ret_type = NULL;
    data_type par_type = NULL;
    int par_num = 0;
    int ret_num = 0;



    GET_NEXT_TOKEN();

    if (!TOK_IS_ID) {
        ERROR = SYNTAX_ERR;
        return false;
    } else {
        item = SYM_FIND();
        item = scope_search(&scope, ID_NAME());
        if(item == NULL){
            item = sym_tab_add_item(top_table(scope), ID_NAME());
            sym_tab_add_data_function(item, ret_type, par_type, true, false, par_num);

            //todo add item into scope
        } else {
            //Zde může nastat problém, pokud funkce může být definovaná, až po deklaraci (logciky mi dává, že by neměla)
            ERROR = UNDEFINED_ERR;
            return false;
        }
    }


    GET_NEXT_TOKEN();
    if (!TOK_IS_TYPE(TOKEN_TYPE_DEF)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    GET_NEXT_TOKEN();
    if (!TOK_IS_KW(KW_FUNCTION)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    GET_NEXT_TOKEN();
    if (!TOK_IS_TYPE(TOKEN_TYPE_LEFTB)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    if (!type_list(&par_type, &par_num)) {
        return false;
    }
    if(!sym_tab_add_data_function(item, ret_type, par_type, true, false, par_num)){
        ERROR = INTERNAL_ERR;
        return false;
    }
//    GET_NEXT_TOKEN();
    if (!TOK_IS_TYPE(TOKEN_TYPE_RIGHTB)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    if (!ret_type_list(&ret_type, &ret_num)) {
        return false;
    }

    if(!sym_tab_add_data_function(item, ret_type, par_type, true, false, par_num)){
        ERROR = INTERNAL_ERR;
        return false;
    }

    if (!body())
        return false;


    return true;
}

bool id_def() {
    GET_NEXT_TOKEN();
    if (!TOK_IS_TYPE(TOKEN_TYPE_LEFTB)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    if (!term_list()) {
        return false;
    }
    GET_NEXT_TOKEN();
    if (!TOK_IS_TYPE(TOKEN_TYPE_RIGHTB)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    if (!body())
        return false;
    return true;

}

bool param_list(data_type *par_type, int *num) {
    GET_NEXT_TOKEN();
    if (TOK_IS_TYPE(TOKEN_TYPE_RIGHTB)) {
        return true;
    }
    if(!TOK_IS_ID) {ERROR = SYNTAX_ERR; return false; }
    GET_NEXT_TOKEN();
    if(!TOK_IS_TYPE(TOKEN_TYPE_DEF)) {ERROR = SYNTAX_ERR; return false; }
    GET_NEXT_TOKEN();
    if(!is_type())
        return false;
    *par_type = create_data_type(get_datatype());
    *(num++);

    if(next_param(par_type,num))
        return true;
    return false;
}

bool next_param(data_type *par_type, int *num) {
    GET_NEXT_TOKEN();
    if (TOK_IS_TYPE(TOKEN_TYPE_RIGHTB)) {
        return true;
    }
    if(!TOK_IS_TYPE(TOKEN_TYPE_COLON)) {
        ERROR = SYNTAX_ERR;
        return false;
    }

    GET_NEXT_TOKEN();
    if(!TOK_IS_ID)
        return false;
    GET_NEXT_TOKEN();
    if(!TOK_IS_TYPE(TOKEN_TYPE_DEF)) 
    {
        ERROR = SYNTAX_ERR; return false;
    }
    GET_NEXT_TOKEN();
    if(!is_type())
        return false;
    add_data_type(*par_type, get_datatype());
    *(num++);
    if(next_param(par_type, num))
        return true;
    return false;
}


/**
 * <ret-type-list> -> : <type> <next-type>
 */
bool ret_type_list(data_type *ret_type, int* num) {
    GET_NEXT_TOKEN();
    if(TOK_IS_TYPE(TOKEN_TYPE_RIGHTB) || TOK_IS_KW(KW_GLOBAL) || TOK_IS_KW(KW_FUNCTION) ||TOK_IS_KW(KW_END) ||TOK_IS_TYPE(TOKEN_TYPE_EOF) ||TOK_IS_KW(KW_IF) ||TOK_IS_KW(KW_WHILE) ||TOK_IS_KW(KW_LOCAL) || TOK_IS_KW(KW_RETURN) || TOK_IS_ID)
        return true;

    if(!TOK_IS_TYPE(TOKEN_TYPE_DEF)) { 
        ERROR = SYNTAX_ERR;
        return false;
    }
    

    GET_NEXT_TOKEN();
    if(!is_type())
        return false;
    *ret_type = create_data_type(get_datatype());
    *(num++);

    if(next_type(ret_type, num))
        return true;

    return false;

}

/**
 * <st-list> -> <statement> <st-list>
 */
bool st_list() {
   //Vyvoření nového ramce
   push(&scope);

    while (!TOK_IS_KW(KW_END) && !TOK_IS_KW(KW_ELSE)){

        if (TOK_IS_KW(KW_IF)) {
            if(!st_if())
                return false;
        } else if (TOK_IS_KW(KW_WHILE)) {
            if(!st_while())
                return false;

        } else if (TOK_IS_KW(KW_LOCAL)){
            if(!st_local())
                return false;
        } else if (TOK_IS_KW(KW_RETURN)) {
            if(!st_return())
                return false;
        } else if (TOK_IS_ID) {


            //TODO
            if(isfunc(&scope, token->attribute.string->s)){
                if (!st_fnc_id()) {
                    ERROR = SYNTAX_ERR;
                    return false;
                }
            }
            else {
                if (!st_var_id()) {
                    ERROR = SYNTAX_ERR;
                    return false;
                }
            }

        } else {
            ERROR = SYNTAX_ERR;
            return false;
        }

    }
    pop(&scope);
    return true;
}

/**
 * <statement> -> local id : <type> = <option> P = local
 */
bool st_local(){
    GET_NEXT_TOKEN();
    if (!TOK_IS_ID) { 
        ERROR = SYNTAX_ERR;
        return false;
    }
    sym_tab_item_t *item_to_add =  NULL;
    data_type par_type = NULL;
    //Zjistí zda se nejedná o redekleraci či redefinici
    if(SYM_FIND() != NULL) {
        ERROR = UNDEFINED_ERR;
        return false;
    }
    
    item_to_add = sym_tab_add_item(top_table(scope), ID_NAME());
    GET_NEXT_TOKEN();
    if(!TOK_IS_TYPE(TOKEN_TYPE_DEF)) {
        ERROR = SYNTAX_ERR;
        return false;
    }

    GET_NEXT_TOKEN();
    if(!is_type())
        return false;

    par_type =  create_data_type(get_datatype());
    sym_tab_add_data_var(item_to_add, par_type, true, false);
    

    GET_NEXT_TOKEN();
    if (TOK_IS_TYPE(TOKEN_TYPE_EQUAL)) {
        //sym_tab_add_data_var(item_to_add, create_data_type(type), true, true);
        GET_NEXT_TOKEN();
        if(TOK_IS_ID){

        if(isfunc(&scope, token->attribute.string->s)){
            if (!st_fnc_id()) {
                ERROR = SYNTAX_ERR;
                return false;
            }
        }
        else {
            if (!st_var_id()) {
                ERROR = SYNTAX_ERR;
                return false;
            }
        }
        } else if (is_type_data()){
            if (!expr()) {
                ERROR = SYNTAX_ERR;
                return false;
            }
        } else {
            ERROR = SYNTAX_ERR;
            return false;
        }
        sym_tab_add_data_var(item_to_add, par_type, true, true);
    } else {
        
    }
    return true;
}

/**
 * <statement> -> if <expr> then <st-list> else <st-list> end
 */
bool st_if(){
    GET_NEXT_TOKEN();
    if (!expr())
        return false;
    if (!TOK_IS_KW(KW_THEN)) { ERROR = SYNTAX_ERR; return false; }
    GET_NEXT_TOKEN();
    if (!st_list())
        return false;
    if (!TOK_IS_KW(KW_ELSE)) { ERROR = SYNTAX_ERR; return false; }
    GET_NEXT_TOKEN();
    if (!st_list())
        return false;
    if (!TOK_IS_KW(KW_END)) { ERROR = SYNTAX_ERR; return false; }
    GET_NEXT_TOKEN();
    return true;
}

/**
 * <statement> -> while <expr> do <st-list> end
 */
bool st_while(){
    GET_NEXT_TOKEN();
    if(!expr())
        return false;
    if(!TOK_IS_KW(KW_DO)) {ERROR = SYNTAX_ERR; return false; }
    GET_NEXT_TOKEN();
    if(!st_list())
        return false;
    if(!TOK_IS_KW(KW_END)) {ERROR = SYNTAX_ERR; return false; }
    GET_NEXT_TOKEN();
    return true;
}

bool st_return(){
    if(!exp_list())
        return false;

    //TODO GET_NEXT_TOKEN();
    return true;
}

//Can be Macro for better usage
bool st_fnc_id(){
    char * name = ID_NAME();
    sym_tab_item_t * item = scope_search(&scope, name);
    GET_NEXT_TOKEN();
    if(!TOK_IS_TYPE(TOKEN_TYPE_LEFTB)){
        ERROR = SYNTAX_ERR;
        return false;
    }

    GET_NEXT_TOKEN();
    data_type par_typy = NULL;
    par_typy = item->data.param_data_types;
    datatypes_list * typ = NULL;

    for(int i = 0; i < item->data.params; i++){
        GET_NEXT_TOKEN();
        if (TOK_IS_TYPE(TOKEN_TYPE_RIGHTB)) {
            ERROR = SYNTAX_ERR;
            return false;
        }

        if (TOK_IS_ID){
            sym_tab_item_t * id = scope_search(&scope, ID_NAME());
            if(id == NULL){
                ERROR = UNDEFINED_ERR;
                return false;
            } else {
                if(!id->data.defined){
                    ERROR = UNDEFINED_ERR;
                    return false;
                }
                typ = item->data.return_data_types->next;

                if(typ->datatype != id->data.return_data_types->datatype) {
                    ERROR = TYPE_INCOMPATIBILITY_ERR;
                    return false;
                }



                GET_NEXT_TOKEN();

                if (TOK_IS_TYPE(TOKEN_TYPE_RIGHTB) && i == item->data.params - 1){
                    break;
                } else if (TOK_IS_TYPE(TOKEN_TYPE_RIGHTB)){
                    ERROR = PARAMETERS_ERR;
                    return false;
                } else if (TOK_IS_TYPE(TOKEN_TYPE_COLON)){
                    continue;
                } else {
                    ERROR = SYNTAX_ERR;
                    return false;
                }



            }
        }
    }


    return true;
}


bool st_var_id(){
    //TODO Možná lepší v id_list/next_id?
    while (sym_tab_find_in_table(top_table(scope), ID_NAME()) != NULL) {
        GET_NEXT_TOKEN();
        if(TOK_IS_TYPE(TOKEN_TYPE_EQUAL)) {
            //Poslat další token nebo to nechat na parseru?
            return expr();
        }

    }
    return false;
}

bool exp_list() {
    GET_NEXT_TOKEN();
    if(!expr())
        return false;

    return next_exp();
}

bool next_exp(){

    //TODO added else to next_exp predict, is it ok? yep
    if(TOK_IS_KW(KW_IF) || TOK_IS_KW(KW_ELSE) ||TOK_IS_KW(KW_WHILE) ||TOK_IS_KW(KW_LOCAL) || TOK_IS_KW(KW_RETURN) || TOK_IS_ID || TOK_IS_KW(KW_END))
        return true;

    if(!TOK_IS_TYPE(TOKEN_TYPE_COLON)) {
        ERROR = SYNTAX_ERR;
        return false;
    }

    return expr();
}

bool option(){
    //TODO Check if function or variable
    //Have to add two other function for function ID case and variable case
    GET_NEXT_TOKEN();
    return true;
}

/*
 * Expression parser
 * Return: true if expression is correctly given
 *         false if there is mistake
 *  Notes: Works separatedly from main rules, must be called by author
 */
bool expr() {
   //TODO dodělat pro input ID a kontrolu jejich typy + vyřešit problém s konecm expressionu a dojetím dalšího kw

    ERROR = parse_expr(token);
    if (ERROR == 0) {
        return true;
    }
    return false;
}

bool type_list(data_type *types, int * num) {
    GET_NEXT_TOKEN();
    if(TOK_IS_TYPE(TOKEN_TYPE_RIGHTB))
        return true;
    if(!is_type())
        return false;
    *types = create_data_type(get_datatype());
    if(next_type(types, num))
        return true;
    return false;
}

bool next_type(data_type *types, int * num){
    GET_NEXT_TOKEN();
    if(TOK_IS_TYPE(TOKEN_TYPE_RIGHTB) || TOK_IS_KW(KW_GLOBAL) || TOK_IS_KW(KW_FUNCTION) ||TOK_IS_KW(KW_END) ||TOK_IS_TYPE(TOKEN_TYPE_EOF) ||TOK_IS_KW(KW_IF) ||TOK_IS_KW(KW_WHILE) ||TOK_IS_KW(KW_LOCAL) || TOK_IS_KW(KW_RETURN) || TOK_IS_ID)
        return true;
    if(!TOK_IS_TYPE(TOKEN_TYPE_COLON)){
        ERROR = SYNTAX_ERR;
        return false;
    }
    GET_NEXT_TOKEN();
    if(!is_type())
        return false;
    *types = add_data_type(*types, get_datatype());
    *(num++);
   return next_type(types, num);

}

bool is_type (){
    if(TOK_IS_KW(KW_INTEGER) || TOK_IS_KW(KW_NUMBER) || TOK_IS_KW(KW_STRING))
        return true;
    ERROR = SYNTAX_ERR;
    return false;
}

bool is_type_data(){
    if(TOK_IS_TYPE(TOKEN_TYPE_INT) || TOK_IS_TYPE(TOKEN_TYPE_STR)  || TOK_IS_TYPE(TOKEN_TYPE_DOUBLE) )
        return true;
    ERROR = SYNTAX_ERR;
    return false;
}

sym_tab_datatype get_datatype(){
    if (TOK_IS_KW(KW_INTEGER))
        return INTEGER;
    else if (TOK_IS_KW(KW_NUMBER))
        return NUMBER;
    else if (TOK_IS_KW(KW_STRING))
        return STRING;

    return NIL;
}

// first id is check at call site
bool id_list(){
   return next_id();
}

bool next_id(){
    GET_NEXT_TOKEN();
    if (TOK_IS_TYPE(TOKEN_TYPE_EQUAL))
        return true;
    if (!TOK_IS_TYPE(TOKEN_TYPE_COLON)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    return next_id();
}
//TODO překopat na zjišťování správných parametrů
bool term_list() {
    GET_NEXT_TOKEN();
    if(!is_term()){
        return false;
    }
    if(!next_term())
        return false;

    return true;
}

bool next_term() {
    GET_NEXT_TOKEN();
    if(!TOK_IS_TYPE(TOKEN_TYPE_COLON)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    GET_NEXT_TOKEN();
    if(!is_term()){
        return false;
    }
    if(!next_term())
        return false;

    return true;
}

bool is_term() {
    if (TOK_IS_TYPE(TOKEN_TYPE_STR) || TOK_IS_TYPE(TOKEN_TYPE_INT) || TOK_IS_TYPE(TOKEN_TYPE_DOUBLE) || TOK_IS_KW(KW_NIL) || TOK_IS_ID){
        return true;
    }
    ERROR = SYNTAX_ERR;
    return false;
}

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

int parse()
{
    token = malloc(sizeof(token_t));

    int result = program();

    free(token);

    sym_tab_t *tab = top_table(scope);
    if (tab)
        sym_tab_free(tab);

    free(scope);
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

bool fnc_def() {
    GET_NEXT_TOKEN();
    if (!TOK_IS_ID) {
        return false;
    } else {
        sym_tab_item_t *item = SYM_FIND();
        if(item == NULL){
            item = sym_tab_add_item(top_table(scope), ID_NAME());
            //todo add item into scope
        }
    }

    GET_NEXT_TOKEN();
    if (!TOK_IS_TYPE(TOKEN_TYPE_LEFTB)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    if (!param_list()) {
        return false;
    }
//    GET_NEXT_TOKEN();
    if (!TOK_IS_TYPE(TOKEN_TYPE_RIGHTB)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    if (!ret_type_list()) {
        return false;
    }
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

    GET_NEXT_TOKEN();

    if (!TOK_IS_ID) {
        ERROR = SYNTAX_ERR;
        return false;
    }
/*    sym_tab_item_t *item_to_add;
    item_to_add = sym_tab_add_item(top_table(scope), "f");
*/

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
    if (!type_list()) {
        return false;
    }
//    GET_NEXT_TOKEN();
    if (!TOK_IS_TYPE(TOKEN_TYPE_RIGHTB)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    if (!ret_type_list()) {
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

bool param_list() {
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
    if(next_param())
        return true;
    return false;
}

bool next_param() {
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
    if(!TOK_IS_TYPE(TOKEN_TYPE_DEF)) {ERROR = SYNTAX_ERR; return false; }
    GET_NEXT_TOKEN();
    if(!is_type())
        return false;
    if(next_param())
        return true;
    return false;
}


/**
 * <ret-type-list> -> : <type> <next-type>
 */
bool ret_type_list() {
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

    if(next_type())
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
            if(isfunc(top_table(scope), token->attribute.string->s)){
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
    sym_tab_item_t * item_to_add;
    //item_to_add = sym_tab_add_item(top_table(scope), token->attribute.string->s);

    GET_NEXT_TOKEN();
    if(!TOK_IS_TYPE(TOKEN_TYPE_DEF)) {
        ERROR = SYNTAX_ERR;
        return false;
    }

    GET_NEXT_TOKEN();
    if(!is_type())
        return false;

    sym_tab_datatype type = get_datatype();

    GET_NEXT_TOKEN();
    if (TOK_IS_TYPE(TOKEN_TYPE_EQUAL)) {
        //sym_tab_add_data_var(item_to_add, create_data_type(type), true, true);

        if(!expr())
            return false;
        //        if(expr(&token, scope, &result)) //TODO
    } else {
        //sym_tab_add_data_var(item_to_add, create_data_type(type), true, true);
    }
    return true;
}

/**
 * <statement> -> if <expr> then <st-list> else <st-list> end
 */
bool st_if(){
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

    return false;
}


bool st_var_id(){
    //TODO Možná lepší v id_list/next_id?
    while (sym_tab_find_in_table(top_table(scope), token->attribute.string->s) != NULL) {
        GET_NEXT_TOKEN();
        if(TOK_IS_TYPE(TOKEN_TYPE_EQUAL)) {
            //Poslat další token nebo to nechat na parseru?
            return expr();
        }

    }
    return false;
}

bool exp_list() {
    // GET_NEXT_TOKEN(); if we call this here expr skips one token
    if(!expr())
        return false;

    return next_exp();
}

bool next_exp(){

    //TODO added else to next_exp predict, is it ok?
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

    GET_NEXT_TOKEN();
    ERROR = parse_expr(token);
    if (ERROR == 0) {
        return true;
    }
    return false;
}

bool type_list() {
    GET_NEXT_TOKEN();
    if(TOK_IS_TYPE(TOKEN_TYPE_RIGHTB))
        return true;
    if(!is_type())
        return false;

    if(next_type())
        return true;
    return false;
}

bool next_type(){
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
   return next_type();

}

bool is_type (){
    if(TOK_IS_KW(KW_INTEGER) || TOK_IS_KW(KW_NUMBER) || TOK_IS_KW(KW_STRING))
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

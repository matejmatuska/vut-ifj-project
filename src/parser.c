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

#define  TOK_IS_TYPE(kw) \
(token->type == (kw))

#define TOK_IS_OP() \
(token->type == TOKEN_TYPE_MINUS || token->type == TOKEN_TYPE_PLUS || token->type == TOKEN_TYPE_DIV_SIGN || token->type == TOKEN_TYPE_MUL_SIGN )

#define SYM_FIND() \
(sym_tab_find_in_table(top_table(scope), token->attribute.string->s))

#define ID_NAME() \
(token->attribute.string->s)

int ERROR = 0;
token_t *token;

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
    sym_tab_free(top_table(scope));
    free(scope);
    return result;
}

// <prog> -> require STRING <body> EOF
int program() {

    get_next_token(token);

    if (!TOK_IS_KW(KW_REQUIRE)) {
        return ERROR;
    }

    get_next_token(token);
    if (token->type != TOKEN_TYPE_STR
        && !dyn_str_compare(token->attribute.string, "ifj21"))
        return ERROR;

    if (!body())
        return ERROR;

    get_next_token(token);
    if (token->type != TOKEN_TYPE_EOF) {
        ERROR = SYNTAX_ERR;
    };

    return ERROR;
}

bool body() {
    push(&scope);
    if(TOK_IS_KW(KW_GLOBAL) || TOK_IS_KW(KW_FUNCTION) || TOK_IS_TYPE(TOKEN_TYPE_EOF) || TOK_IS_ID){

    } else
        get_next_token(token);
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
    get_next_token(token);
    if (!TOK_IS_ID) {
        return false;
    } else {
        sym_tab_item_t *item = SYM_FIND();
        if(item == NULL){
            item = sym_tab_add_item(top_table(scope), ID_NAME());
            //todo add item into scope
        }
    }

    get_next_token(token);
    if (!TOK_IS_TYPE(TOKEN_TYPE_LEFTB)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    if (!param_list()) {
        return false;
    }
//    get_next_token(token);
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
    get_next_token(token);
    if (!TOK_IS_ID) {
        ERROR = SYNTAX_ERR;
        return false;
    }
/*    sym_tab_item_t *item_to_add;
    item_to_add = sym_tab_add_item(top_table(scope), "f");
*/

    get_next_token(token);
    if (!TOK_IS_TYPE(TOKEN_TYPE_DEF)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    get_next_token(token);
    if (!TOK_IS_KW(KW_FUNCTION)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    get_next_token(token);
    if (!TOK_IS_TYPE(TOKEN_TYPE_LEFTB)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    if (!type_list()) {
        return false;
    }
//    get_next_token(token);
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
    get_next_token(token);
    if (!TOK_IS_TYPE(TOKEN_TYPE_LEFTB)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    if (!term_list()) {
        return false;
    }
    get_next_token(token);
    if (!TOK_IS_TYPE(TOKEN_TYPE_RIGHTB)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    if (!body())
        return false;
    return true;

}

bool param_list() {
    get_next_token(token);
    if (TOK_IS_TYPE(TOKEN_TYPE_RIGHTB)) {
        return true;
    }
    if(!TOK_IS_ID) {ERROR = SYNTAX_ERR; return false; }
    get_next_token(token);
    if(!TOK_IS_TYPE(TOKEN_TYPE_DEF)) {ERROR = SYNTAX_ERR; return false; }
    get_next_token(token);
    if(!is_type())
        return false;
    if(next_param())
        return true;
    return false;
}

bool next_param() {
    get_next_token(token);
    if (TOK_IS_TYPE(TOKEN_TYPE_RIGHTB)) {
        return true;
    }
    if(!TOK_IS_TYPE(TOKEN_TYPE_COLON)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    get_next_token(token);
    if(!TOK_IS_ID)
        return false;
    get_next_token(token);
    if(!TOK_IS_TYPE(TOKEN_TYPE_DEF)) {ERROR = SYNTAX_ERR; return false; }
    get_next_token(token);
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
    get_next_token(token);
    if(TOK_IS_TYPE(TOKEN_TYPE_RIGHTB) || TOK_IS_KW(KW_GLOBAL) || TOK_IS_KW(KW_FUNCTION) ||TOK_IS_KW(KW_END) ||TOK_IS_TYPE(TOKEN_TYPE_EOF) ||TOK_IS_KW(KW_IF) ||TOK_IS_KW(KW_WHILE) ||TOK_IS_KW(KW_LOCAL) || TOK_IS_KW(KW_RETURN) || TOK_IS_ID)
        return true;

    if(!TOK_IS_TYPE(TOKEN_TYPE_DEF)) { 
        ERROR = SYNTAX_ERR;
        return false;
    }

    get_next_token(token);
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

        if(TOK_IS_KW(KW_IF)) {
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


            //TODO ???
            if(isfunc(top_table(scope), token->attribute.string->s)){
                st_fnc_id();
            }
            else {
                st_var_id();
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
    get_next_token(token);
    if(!TOK_IS_ID) {ERROR = SYNTAX_ERR; return false; }
    sym_tab_item_t * item_to_add;
    item_to_add = sym_tab_add_item(top_table(scope), token->attribute.string->s);

    get_next_token(token);
    if(!TOK_IS_TYPE(TOKEN_TYPE_DEF)) {
        ERROR = SYNTAX_ERR;
        return false;
    }

    get_next_token(token);
    if(!is_type())
        return false;


    sym_tab_datatype type = get_datatype();

    get_next_token(token);
    if(TOK_IS_TYPE(TOKEN_TYPE_EQUAL)) {
        sym_tab_add_data(item_to_add, HT_VAR, type, true, 0);

        if(!expr())
            return false;
//        if(expr(&token, scope, &result)) //TODO
    } else
        sym_tab_add_data(item_to_add, HT_VAR, type, false, 0); //Možná úprava?
    return true;
}

/**
 * <statement> -> if <expr> then <st-list> else <st-list> end
 */
bool st_if(){
    if(!expr())
        return false;
    if(!TOK_IS_KW(KW_THEN)) {ERROR = SYNTAX_ERR; return false; }
    get_next_token(token);
    if(!st_list())
        return false;
    if(!TOK_IS_KW(KW_ELSE)) {ERROR = SYNTAX_ERR; return false; }
    get_next_token(token);
    if(!st_list())
        return false;
    if(!TOK_IS_KW(KW_END)) {ERROR = SYNTAX_ERR; return false; }
    get_next_token(token);
    return true;
}

/**
 * <statement> -> while <expr> do <st-list> end
 */
bool st_while(){
    if(!expr())
        return false;
    if(!TOK_IS_KW(KW_DO)) {ERROR = SYNTAX_ERR; return false; }
    get_next_token(token);
    if(!st_list())
        return false;
    if(!TOK_IS_KW(KW_END)) {ERROR = SYNTAX_ERR; return false; }
    get_next_token(token);
    return true;
}

bool st_return(){
    if(!exp_list())
        return false;
    get_next_token(token);
    return true;
}

//Can be Macro for better usage
bool st_fnc_id(){

    return false;
}


bool st_var_id(){
    //TODO Možná lepší v id_list/next_id?
    while (sym_tab_find_in_table(top_table(scope), token->attribute.string->s) != NULL) {
        get_next_token(token);
        if(TOK_IS_TYPE(TOKEN_TYPE_EQUAL)) {
            //Poslat další token nebo to nechat na parseru?
            if(expr())
                return true;
            return false;
        }

    }
    return false;
}

bool exp_list() {
    get_next_token(token);
    if(!expr())
        return false;
    if(!next_exp())      //TODO nejspíš se přeskočí token not-tested
        return false;
    return true;

}

bool next_exp(){
    get_next_token(token);

    if(TOK_IS_KW(KW_IF) ||TOK_IS_KW(KW_WHILE) ||TOK_IS_KW(KW_LOCAL) || TOK_IS_KW(KW_RETURN) || TOK_IS_ID || TOK_IS_KW(KW_END))
        return true;

    if(!TOK_IS_TYPE(TOKEN_TYPE_COLON)) {
        ERROR = SYNTAX_ERR;
        return false; }

    get_next_token(token);
    if(!expr())
        return false;

    return true;
}

bool option(){
    //TODO Check if function or variable
    //Have to add two other function for function ID case and variable case
    get_next_token(token);
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

    get_next_token(token);
    ERROR = parse_expr(token);
    if (ERROR == 0) {
        return true;
    }
    return false;
}

bool type_list() {
    get_next_token(token);
    if(TOK_IS_TYPE(TOKEN_TYPE_RIGHTB))
        return true;
    if(!is_type())
        return false;

    if(next_type())
        return true;
    return false;
}

bool next_type(){
    get_next_token(token);
    if(TOK_IS_TYPE(TOKEN_TYPE_RIGHTB) || TOK_IS_KW(KW_GLOBAL) || TOK_IS_KW(KW_FUNCTION) ||TOK_IS_KW(KW_END) ||TOK_IS_TYPE(TOKEN_TYPE_EOF) ||TOK_IS_KW(KW_IF) ||TOK_IS_KW(KW_WHILE) ||TOK_IS_KW(KW_LOCAL) || TOK_IS_KW(KW_RETURN) || TOK_IS_ID)
        return true;
    if(!TOK_IS_TYPE(TOKEN_TYPE_COLON)){
        ERROR = SYNTAX_ERR;
        return false;
    }
    get_next_token(token);
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

bool id_list(){
   if(!next_id())
       return false;
    return true;
}

bool next_id(){
    get_next_token(token);
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
    get_next_token(token);
    if(!is_term()){
        return false;
    }
    if(!next_term())
        return false;

    return true;
}

bool next_term() {
    get_next_token(token);
    if(!TOK_IS_TYPE(TOKEN_TYPE_COLON)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    get_next_token(token);
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

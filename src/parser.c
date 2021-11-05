#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "scanner.h"
#include "dynamic_string.h"
#include "parser.h"

#define TOK_IS_KW(kw) \
(token->type == TOKEN_TYPE_KW && token->attribute.keyword == (kw))

#define TOK_IS_ID \
(token->type == TOKEN_TYPE_ID)

#define  TOK_IS_TYPE(kw) \
(token->type == kw)

token_t *token;




// <prog> -> require STRING <body> EOF
bool program() {
    // init token
    token = malloc(sizeof(token_t));
    get_next_token(token);

    // check for require
    if (!TOK_IS_KW(KW_REQUIRE)) {
        return false;
    }
    // check for "ifj21"
    get_next_token(token);
    if (token->type != TOKEN_TYPE_STR
        && !dyn_str_compare(token->attribute.string, "ifj21"))
        return false;

    // check <body> nonterminal
    if (!body())
        return false;

    // check EOF
    get_next_token(token);
    bool eof_res = token->type == TOKEN_TYPE_EOF;

    free(token);
    return eof_res;
}

bool body() {
    if(TOK_IS_KW(KW_GLOBAL) || TOK_IS_KW(KW_FUNCTION) || TOK_IS_TYPE(TOKEN_TYPE_EOF) || TOK_IS_ID){

    } else
        get_next_token(token);
    if (TOK_IS_KW(KW_GLOBAL)) {
        if (glob_def())
            return true;
        return false;
    } else if (TOK_IS_KW(KW_FUNCTION)) {
        if (fnc_def())
            return true;
        return false;
    } else if (token->type == TOKEN_TYPE_ID) {
        if (id_def())
            return true;
        return false;
    } else if (token->type == TOKEN_TYPE_EOF) {
        return true;
    }
    return false;
}

bool fnc_def() {
    get_next_token(token);
    if (!TOK_IS_ID) {
        return false;
    }
    get_next_token(token);
    if (!TOK_IS_TYPE(TOKEN_TYPE_LEFTB)) {
        return false;
    }
    if (!param_list()) {
        return false;
    }
//    get_next_token(token);
    if (!TOK_IS_TYPE(TOKEN_TYPE_RIGHTB)) {
        return false;
    }
    if (!ret_type_list()) {
        return false;
    }
    if (!st_list()) {
        return false;
    }
    if (!TOK_IS_KW(KW_END)) {
        return false;
    }
    if (!body())
        return false;
    return true;
}

bool glob_def() {
    get_next_token(token);
    if (!TOK_IS_ID) {
        return false;
    }
    get_next_token(token);
    if (!TOK_IS_TYPE(TOKEN_TYPE_DEF)) {
        return false;
    }
    get_next_token(token);
    if (!TOK_IS_KW(KW_FUNCTION)) {
        return false;
    }
    get_next_token(token);
    if (!TOK_IS_TYPE(TOKEN_TYPE_LEFTB)) {
        return false;
    }
    if (!type_list()) {
        return false;
    }
//    get_next_token(token);
    if (!TOK_IS_TYPE(TOKEN_TYPE_RIGHTB)) {
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
        return false;
    }
    if (!term_list()) {
        return false;
    }
    get_next_token(token);
    if (!TOK_IS_TYPE(TOKEN_TYPE_RIGHTB)) {
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
    if(!TOK_IS_ID)
        return false;
    get_next_token(token);
    if(!TOK_IS_TYPE(TOKEN_TYPE_DEF))
        return false;
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
        return false;
    }
    get_next_token(token);
    if(!TOK_IS_ID)
        return false;
    get_next_token(token);
    if(!TOK_IS_TYPE(TOKEN_TYPE_DEF))
        return false;
    get_next_token(token);
    if(!is_type())
        return false;
    if(next_param())
        return true;
    return false;
}

bool ret_type_list() {
    get_next_token(token);
    if(TOK_IS_TYPE(TOKEN_TYPE_RIGHTB) || TOK_IS_KW(KW_GLOBAL) || TOK_IS_KW(KW_FUNCTION) ||TOK_IS_KW(KW_END) ||TOK_IS_TYPE(TOKEN_TYPE_EOF) ||TOK_IS_KW(KW_IF) ||TOK_IS_KW(KW_WHILE) ||TOK_IS_KW(KW_LOCAL) || TOK_IS_KW(KW_RETURN) || TOK_IS_ID)
        return true;
    if(!TOK_IS_TYPE(TOKEN_TYPE_DEF))
        return false;
    get_next_token(token);
    if(!is_type())
        return false;
    if(next_type())
        return true;
    return false;

}

bool st_list() {
    if (TOK_IS_KW(KW_END) || TOK_IS_KW(KW_ELSE)) {

        return true;
    }
   //     get_next_token(token);




    if(TOK_IS_KW(KW_IF)) {
        if(!st_if())
            return false;
        if(st_list())
            return true;
        return false;

    } else if (TOK_IS_KW(KW_WHILE)) {
        if(!st_while())
            return false;
        if(st_list())
            return true;
        return false;
    } else if (TOK_IS_KW(KW_LOCAL)){
        if(!st_local())
            return false;
        if(st_list())
            return true;
        return false;
    } else if (TOK_IS_KW(KW_RETURN)) {
        if(!st_return())
            return false;
        if(st_list())
            return true;
        return false;
    } else if (TOK_IS_ID){
        //TODO Check if function or variable
        st_fnc_id();
        st_var_id();

        if(st_list())
            return true;
        return false;
    }
    return false;

}

bool st_local(){
    get_next_token(token);
    if(!TOK_IS_ID)
        return false;
    get_next_token(token);
    if(!TOK_IS_TYPE(TOKEN_TYPE_DEF))
        return false;
    get_next_token(token);
    if(!is_type())
        return false;
    get_next_token(token);
    if(!TOK_IS_TYPE(TOKEN_TYPE_EQUAL))
        return false;
    if(!option())
        return false;
    return true;
}

bool st_if(){
    if(!expr())
        return false;
    if(!TOK_IS_KW(KW_THEN))
        return false;
    get_next_token(token);
    if(!st_list())
        return false;
    if(!TOK_IS_KW(KW_ELSE))
        return false;
    get_next_token(token);
    if(!st_list())
        return false;
    if(!TOK_IS_KW(KW_END))
        return false;
    get_next_token(token);
    return true;
}

bool st_while(){
    if(!expr())
        return false;
    if(!TOK_IS_KW(KW_DO))
        return false;
    get_next_token(token);
    if(!st_list())
        return false;
    if(!TOK_IS_KW(KW_END))
        return false;
    get_next_token(token);
    return true;
}

bool st_return(){
    if(!exp_list())
        return false;
    get_next_token(token);
    return true;
}

bool st_fnc_id(){

}

bool st_var_id(){

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

    if(!TOK_IS_TYPE(TOKEN_TYPE_COLON))
        return false;
    get_next_token(token);
    if(!expr())
        return false;



    return true;
}

bool option(){
    //TODO Check if function or variable
    get_next_token(token);
    return true;
}

bool expr() {
    get_next_token(token);
    return true;
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
        return false;
    }
    get_next_token(token);
    if(!is_type())
        return false;
    next_type();

}

bool is_type (){
    if(TOK_IS_KW(KW_INTEGER) || TOK_IS_KW(KW_NUMBER) || TOK_IS_KW(KW_STRING))
        return true;
    return false;
}

bool id_list(){
   if(!next_id())
       return false;
}

bool next_id(){
    get_next_token(token);
    if (TOK_IS_TYPE(TOKEN_TYPE_EQUAL))
        return true;
    if(!TOK_IS_TYPE(TOKEN_TYPE_COLON))
        return false;

    return true;
}

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
    if(!TOK_IS_TYPE(TOKEN_TYPE_COLON))
        return false;
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
    return false;
}
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "parser.h"
#include "expr_parser.h"

#include "scanner.h"
#include "dynamic_string.h"
#include "symtable.h"
#include "ST_stack.h"
#include "symbol_stack.h"

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

ST_stack *scope = NULL;

sym_tab_item_t * act_fnc = NULL;

sym_tab_datatype get_datatype();

int program();

bool body();

bool fnc_def();

bool glob_def();

bool id_def();

bool param_list(data_type *par_type, int *num);

bool next_param(data_type *par_type, int *num);

bool ret_type_list(data_type *ret_type, int *num);

bool st_list();

bool st_local();

bool st_if();

bool st_while();

bool st_return();

bool fnc_id();

bool st_fnc_id(name_and_data *var_type, int *var_num);

bool st_var_id();

bool st_next_var_id(name_and_data *types, int *num);

bool option();

bool expr(name_and_data *types, int * num);

bool exp_list(name_and_data *types, int * num);

bool next_exp(name_and_data *types, int * num);

bool type_list(data_type *types, int *num);

bool next_type(data_type *types, int *num);

bool is_type();

sym_tab_datatype get_type_to_sym_type();

bool term_list();

bool next_term();

bool is_term();

bool id_list(name_and_data *types, int *num);

bool next_id(name_and_data *types, int *num);

bool is_type_data();

data_type name_to_type(name_and_data nameAndData);
data_type_t sym_data_to_data_type (sym_tab_datatype data);

int parse() {
    scope = init_ST_stack();
    token = malloc(sizeof(token_t));
    token_init(token);

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
    push(&scope);
    if (!body())
        return ERROR;

    GET_NEXT_TOKEN();
    if (token->type != TOKEN_TYPE_EOF) {
        ERROR = SYNTAX_ERR;
    };

    return ERROR;
}

bool body() {

    if (TOK_IS_KW(KW_GLOBAL) || TOK_IS_KW(KW_FUNCTION) || TOK_IS_TYPE(TOKEN_TYPE_EOF) || TOK_IS_ID) {

    } else
        GET_NEXT_TOKEN();
    if (TOK_IS_KW(KW_GLOBAL)) {
        return glob_def();
    } else if (TOK_IS_KW(KW_FUNCTION)) {
        return fnc_def();
    } else if (token->type == TOKEN_TYPE_ID) {
        if (isfunc(&scope, ID_NAME()) == true) {
            return fnc_id();
        }
        ERROR = UNDEFINED_ERR;
        return false;
    } else if (token->type == TOKEN_TYPE_EOF) {
        return true;
    } else {
        ERROR = SYNTAX_ERR;
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
    bool exist = false;
    if (!TOK_IS_ID) {
        return false;
    } else {
        item = SYM_FIND();
        if (item == NULL) {
            item = sym_tab_add_item(top_table(scope), ID_NAME());
            sym_tab_add_data_function(item, ret_type, par_type, true, true, par_num, ret_num);
        } else {
            exist = true;
        }
    }

    push(&scope);

    GET_NEXT_TOKEN();
    if (!TOK_IS_TYPE(TOKEN_TYPE_LEFTB)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    //create_data_type(par_type);


    if (!param_list(&par_type,
                    &par_num)) {
        return false;
    }
    if (!exist) {
        sym_tab_add_data_function(item, ret_type, par_type, true, true,
                                  par_num, ret_num);
    } else {
        while (par_type != NULL && item->data.param_data_types != NULL) {
            if (item->data.param_data_types->datatype == par_type->datatype) {
                item->data.param_data_types = item->data.param_data_types->next;
                par_type = par_type->next;
            } else {
                ERROR = UNDEFINED_ERR;
                return false;
            }
        }
        if (par_type != NULL || item->data.param_data_types != NULL) {
            ERROR = UNDEFINED_ERR;
            return false;
        }

    }
//    GET_NEXT_TOKEN();
    if (!TOK_IS_TYPE(TOKEN_TYPE_RIGHTB)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    //create_data_type(ret_type);
    if (!ret_type_list(&ret_type, &ret_num)) {
        return false;
    }

    if (!exist) {
        sym_tab_add_data_function(item, ret_type, par_type, true, true,
                                  par_num, ret_num);
    } else {
        while (ret_type != NULL && item->data.return_data_types != NULL) {
            if (item->data.return_data_types->datatype == ret_type->datatype) {
                item->data.return_data_types = item->data.return_data_types->next;
                ret_type = ret_type->next;
            } else {
                ERROR = UNDEFINED_ERR;
                return false;
            }
        }
        if (ret_type != NULL || item->data.return_data_types != NULL) {
            ERROR = UNDEFINED_ERR;
            return false;
        }

    }
    act_fnc = item;

    if (!st_list()) {
        return false;
    }
    if (!TOK_IS_KW(KW_END)) {
        ERROR = SYNTAX_ERR;
        return false;
    }



    pop(&scope);

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
        if (item == NULL) {
            item = sym_tab_add_item(top_table(scope), ID_NAME());
            sym_tab_add_data_function(item, ret_type, par_type, true, false, par_num, ret_num);

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
    if (!sym_tab_add_data_function(item, ret_type, par_type, true, false, par_num, ret_num)) {
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

    if (!sym_tab_add_data_function(item, ret_type, par_type, true, false, par_num, ret_num)) {
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
    if (!TOK_IS_ID) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    sym_tab_item_t *item;
    item = sym_tab_add_item(top_table(scope), ID_NAME());

    GET_NEXT_TOKEN();
    if (!TOK_IS_TYPE(TOKEN_TYPE_DEF)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    GET_NEXT_TOKEN();
    if (!is_type())
        return false;
    *par_type = create_data_type(get_datatype());
    (*num)++;
    sym_tab_add_data_var(item, create_data_type(get_datatype()), true, false);


    if (next_param(par_type, num))
        return true;
    return false;
}

bool next_param(data_type *par_type, int *num) {
    GET_NEXT_TOKEN();
    if (TOK_IS_TYPE(TOKEN_TYPE_RIGHTB)) {
        return true;
    }
    if (!TOK_IS_TYPE(TOKEN_TYPE_COLON)) {
        ERROR = SYNTAX_ERR;
        return false;
    }

    GET_NEXT_TOKEN();
    if (!TOK_IS_ID)
        return false;

    sym_tab_item_t *item;
    item = sym_tab_add_item(top_table(scope), ID_NAME());

    GET_NEXT_TOKEN();
    if (!TOK_IS_TYPE(TOKEN_TYPE_DEF)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    GET_NEXT_TOKEN();
    if (!is_type())
        return false;
    add_data_type(*par_type, get_datatype());
    (*num)++;
    sym_tab_add_data_var(item, create_data_type(get_datatype()), true, false);
    if (next_param(par_type, num))
        return true;
    return false;
}

/**
 * <ret-type-list> -> : <type> <next-type>
 */
bool ret_type_list(data_type *ret_type, int *num) {
    GET_NEXT_TOKEN();
    if (TOK_IS_TYPE(TOKEN_TYPE_RIGHTB) || TOK_IS_KW(KW_GLOBAL) || TOK_IS_KW(KW_FUNCTION) || TOK_IS_KW(KW_END) ||
        TOK_IS_TYPE(TOKEN_TYPE_EOF) || TOK_IS_KW(KW_IF) || TOK_IS_KW(KW_WHILE) || TOK_IS_KW(KW_LOCAL) ||
        TOK_IS_KW(KW_RETURN) || TOK_IS_ID)
        return true;

    if (!TOK_IS_TYPE(TOKEN_TYPE_DEF)) {
        ERROR = SYNTAX_ERR;
        return false;
    }


    GET_NEXT_TOKEN();
    if (!is_type())
        return false;
    *ret_type = create_data_type(get_datatype());
    (*num)++;

    if (next_type(ret_type, num))
        return true;

    return false;

}

/**
 * <st-list> -> <statement> <st-list>
 */
bool st_list() {
    //Vyvoření nového ramce


    if (TOK_IS_KW(KW_END) || TOK_IS_KW(KW_ELSE)) {
        return true;
    }

    if (TOK_IS_KW(KW_IF)) {
        if (!st_if())
            return false;
    } else if (TOK_IS_KW(KW_WHILE)) {
        if (!st_while())
            return false;

    } else if (TOK_IS_KW(KW_LOCAL)) {
        if (!st_local())
            return false;
    } else if (TOK_IS_KW(KW_RETURN)) {
        if (!st_return())
            return false;
    } else if (TOK_IS_ID) {
        if(isfunc(&scope, ID_NAME())){
            fnc_id();
        } else
        if (!st_var_id()) {
            return false;
        }


    } else {
        ERROR = SYNTAX_ERR;
        return false;
    }
    if(!st_list()){
        return false;
    }

    return true;
}

/**
 * <statement> -> local id : <type> = <option> P = local
 */
bool st_local() {
    GET_NEXT_TOKEN();
    if (!TOK_IS_ID) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    sym_tab_item_t *item_to_add = NULL;
    name_and_data par_type = NULL;
    int num = 0;

    dynamic_string_t * name = (dynamic_string_t*)malloc(sizeof(dynamic_string_t));
    dyn_str_init(name);
    dyn_str_add_string(name, ID_NAME());

    //Zjistí zda se nejedná o redekleraci či redefinici
    if (SYM_FIND() != NULL) {
        ERROR = UNDEFINED_ERR;
        return false;
    }

    item_to_add = sym_tab_add_item(top_table(scope), ID_NAME());
    GET_NEXT_TOKEN();
    if (!TOK_IS_TYPE(TOKEN_TYPE_DEF)) {
        ERROR = SYNTAX_ERR;
        return false;
    }

    GET_NEXT_TOKEN();
    if (!is_type())
        return false;



    par_type = create_name_data(get_datatype(), name);
    data_type par_typ = name_to_type(par_type);
    sym_tab_add_data_var(item_to_add, par_typ, true, false);
    num = 1;

    GET_NEXT_TOKEN();
    if (TOK_IS_TYPE(TOKEN_TYPE_EQUAL)) {
        //sym_tab_add_data_var(item_to_add, create_data_type(type), true, true);
        GET_NEXT_TOKEN();
        if (TOK_IS_ID) {

            if (isfunc(&scope, ID_NAME())) {
                if (!st_fnc_id(&par_type, &num)) {
                    return false;
                }
            } else if (isvar(&scope, ID_NAME())){
                if(!expr(&par_type, &num)){
                    return false;
                } else if (num > 0) {
                    ERROR = SEMANTIC_ERR;
                    return false;
                }
            } else {
                ERROR = UNDEFINED_ERR;
                return false;
            }
        } else if (is_type_data()) {
            if (!expr(&par_type, &num)) {
                return false;
            } else if (num > 0) {
            ERROR = SEMANTIC_ERR;
            return false;
        }
        } else {
            ERROR = SYNTAX_ERR;
            return false;
        }
        sym_tab_add_data_var(item_to_add, par_typ, true, true);
    } else {

    }

    return true;
}

/**
 * <statement> -> if <expr> then <st-list> else <st-list> end
 */
bool st_if() {

    name_and_data par_type = NULL;
    dynamic_string_t * name = (dynamic_string_t*)malloc(sizeof(dynamic_string_t));
    dyn_str_init(name);
    dyn_str_add_string(name, "bool");
    par_type = create_name_data(INTEGER, name);
    int num = 1;
    GET_NEXT_TOKEN();
 /*   if (!expr(&par_type, &num)) {
        return false;
    }  else if (num > 0) {
        ERROR = SEMANTIC_ERR;
        return false;
    }
    */

    if (TOK_IS_ID) {

        if (isfunc(&scope, ID_NAME())) {
            if (!st_fnc_id(&par_type, &num)) {
                return false;
            }
        } else if (isvar(&scope, ID_NAME())){
            if(!expr(&par_type, &num)){
                return false;
            } else if (num > 0) {
                ERROR = SEMANTIC_ERR;
                return false;
            }
        } else {
            ERROR = UNDEFINED_ERR;
            return false;
        }
    } else if (is_type_data() || TOK_IS_TYPE(TOKEN_TYPE_LENGTH) || TOK_IS_OP() ||
    TOK_IS_KW(KW_NIL) || TOK_IS_TYPE(TOKEN_TYPE_LEFTB)) {
        if (!expr(&par_type, &num)) {
            return false;
        } else if (num > 0) {
            ERROR = SEMANTIC_ERR;
            return false;
        }
    } else {
        ERROR = SYNTAX_ERR;
        return false;
    }

    if (!TOK_IS_KW(KW_THEN)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    GET_NEXT_TOKEN();
    push(&scope);
    if (!st_list()) {
        pop(&scope);
        return false;
    }
    pop(&scope);
    if (!TOK_IS_KW(KW_ELSE)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    GET_NEXT_TOKEN();
    push(&scope);
    if (!st_list()) {
        pop(&scope);
        return false;
    }

    pop(&scope);
    if (!TOK_IS_KW(KW_END)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    GET_NEXT_TOKEN();
    return true;
}

/**
 * <statement> -> while <expr> do <st-list> end
 */
bool st_while() {
    name_and_data par_type = NULL;
    dynamic_string_t * name = (dynamic_string_t*)malloc(sizeof(dynamic_string_t));
    dyn_str_init(name);
    dyn_str_add_string(name, "bool");
    par_type = create_name_data(INTEGER, name);
    int num = 1;
    GET_NEXT_TOKEN();
  /*  if (!expr(&par_type, &num)) {
        return false;
    } else if (num > 0) {
        ERROR = SEMANTIC_ERR;
        return false;
    } */

    if (TOK_IS_ID) {

        if (isfunc(&scope, ID_NAME())) {
            if (!st_fnc_id(&par_type, &num)) {
                return false;
            }
        } else if (isvar(&scope, ID_NAME())){
            if(!expr(&par_type, &num)){
                return false;
            } else if (num > 0) {
                ERROR = SEMANTIC_ERR;
                return false;
            }
        } else {
            ERROR = UNDEFINED_ERR;
            return false;
        }
    } else if (is_type_data() || TOK_IS_TYPE(TOKEN_TYPE_LENGTH) || TOK_IS_OP() ||
               TOK_IS_KW(KW_NIL) || TOK_IS_TYPE(TOKEN_TYPE_LEFTB)) {
        if (!expr(&par_type, &num)) {
            return false;
        } else if (num > 0) {
            ERROR = SEMANTIC_ERR;
            return false;
        }
    } else {
        ERROR = SYNTAX_ERR;
        return false;
    }

    if (!TOK_IS_KW(KW_DO)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    GET_NEXT_TOKEN();
    push(&scope);
    if (!st_list()) {
        pop(&scope);
        return false;
    }
    pop(&scope);
    if (!TOK_IS_KW(KW_END)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    GET_NEXT_TOKEN();
    return true;
}

bool st_return() {
    //TODO možná udělat vlastní expression vyhodnovač, kvuli absenci jmen pro generaci (možná paramy?) -> poradit se
    name_and_data var_type = NULL;
    sym_tab_item_t * fnc = act_fnc;

    int var_num = 0;
    while (fnc->data.return_data_types != NULL){
        fnc->data.return_data_types = fnc->data.return_data_types->next;
        var_num += 1;
    }
    if (!exp_list(&var_type, &var_num)){
        return false;
    }
    //gen return
    while (var_num > 0) {
        //gen_nil
        var_num -= 1;
    }


    //TODO GET_NEXT_TOKEN();
    return true;
}

//Can be Macro for better usage
bool fnc_id() {
    char *name = ID_NAME();
    sym_tab_item_t *item = scope_search(&scope, name);
    if (item == NULL) {
        ERROR = UNDEFINED_ERR;
        return false;
    }
    GET_NEXT_TOKEN();
    if (!TOK_IS_TYPE(TOKEN_TYPE_LEFTB)) {
        ERROR = SYNTAX_ERR;
        return false;
    }

    // GET_NEXT_TOKEN();
    data_type par_typy = NULL;
    par_typy = item->data.param_data_types;
    datatypes_list *typ = item->data.param_data_types;
    if (item->data.params != 0) {
        for (int i = 0; i < item->data.params; i++) {
            GET_NEXT_TOKEN();
            if (TOK_IS_TYPE(TOKEN_TYPE_RIGHTB)) {
                ERROR = PARAMETERS_ERR;
                return false;
            }


            if (TOK_IS_ID) {
                sym_tab_item_t *id = scope_search(&scope, ID_NAME());
                if (id == NULL) {
                    ERROR = UNDEFINED_ERR;
                    return false;
                } else {
                    if (!id->data.defined) {
                        ERROR = UNDEFINED_ERR;
                        return false;
                    }


                    if (typ->datatype != id->data.return_data_types->datatype) {
                        ERROR = TYPE_INCOMPATIBILITY_ERR;
                        return false;
                    }
                }
            } else if (is_type_data()) {
                if (get_type_to_sym_type() != typ->datatype) {
                    ERROR = TYPE_INCOMPATIBILITY_ERR;
                    return false;
                }
            }

            typ = item->data.param_data_types->next;
            GET_NEXT_TOKEN();

            if (TOK_IS_TYPE(TOKEN_TYPE_RIGHTB) && i == item->data.params - 1) {
                break;
            } else if (TOK_IS_TYPE(TOKEN_TYPE_RIGHTB)) {
                ERROR = PARAMETERS_ERR;
                return false;
            } else if (TOK_IS_TYPE(TOKEN_TYPE_COLON)) {
                if (i == item->data.params - 1) {
                    ERROR = PARAMETERS_ERR;
                    return false;
                }
                continue;
            } else {
                ERROR = SYNTAX_ERR;
                return false;
            }
        }
    } else {
        GET_NEXT_TOKEN();
        if (TOK_IS_TYPE(TOKEN_TYPE_RIGHTB)) {
            return true;
        } else {
            ERROR = PARAMETERS_ERR;
            return false;
        }
    }


    GET_NEXT_TOKEN();
    return true;
}

bool check_returns(name_and_data *var_type, sym_tab_item_t * item, int *var_num){
    if (*var_num > item->data.returns) {
        ERROR = PARAMETERS_ERR;
        return false;
    }else{
        name_and_data tmp_name = *var_type;
        data_type tmp_type = item->data.return_data_types;
        while (tmp_name != NULL){
            if(tmp_type->datatype != tmp_name->datatype){
                ERROR = PARAMETERS_ERR;
                return false;
            }
            tmp_name = tmp_name->next;
            tmp_type = tmp_type->next;
        }
        *var_num -= item->data.returns;
    }

    return true;
}

bool st_fnc_id(name_and_data *var_type, int *var_num) {
    char *name = ID_NAME();
    sym_tab_item_t *item = scope_search(&scope, name);
    if (item == NULL) {
        ERROR = UNDEFINED_ERR;
        return false;
    }



//TODO doplnit podle toho, zda se na jedno přiřazení může volat více proměnných. Pokud ne tak se skotroluje počet returnu a počet požadovaných příkazů.
    //Podle toho potom vyhodnotit návratové hodnoty
    GET_NEXT_TOKEN();
    if (!TOK_IS_TYPE(TOKEN_TYPE_LEFTB)) {
        ERROR = SYNTAX_ERR;
        return false;
    }

    // GET_NEXT_TOKEN();
    data_type par_typy = NULL;
    par_typy = item->data.param_data_types;
    datatypes_list *typ = item->data.param_data_types;
    if (item->data.params != 0) {
        for (int i = 0; i < item->data.params; i++) {
            GET_NEXT_TOKEN();
            if (TOK_IS_TYPE(TOKEN_TYPE_RIGHTB)) {
                ERROR = PARAMETERS_ERR;
                return false;
            }


            if (TOK_IS_ID) {
                sym_tab_item_t *id = scope_search(&scope, ID_NAME());
                if (id == NULL) {
                    ERROR = UNDEFINED_ERR;
                    return false;
                } else {
                    if (!id->data.defined) {
                        ERROR = UNDEFINED_ERR;
                        return false;
                    }


                    if (typ->datatype != id->data.return_data_types->datatype) {
                        ERROR = TYPE_INCOMPATIBILITY_ERR;
                        return false;
                    }
                }
            } else if (is_type_data()) {
                if (get_type_to_sym_type() != typ->datatype) {
                    ERROR = TYPE_INCOMPATIBILITY_ERR;
                    return false;
                }
            }

            typ = item->data.param_data_types->next;
            GET_NEXT_TOKEN();

            if (TOK_IS_TYPE(TOKEN_TYPE_RIGHTB) && i == item->data.params - 1) {
                break;
            } else if (TOK_IS_TYPE(TOKEN_TYPE_RIGHTB)) {
                ERROR = PARAMETERS_ERR;
                return false;
            } else if (TOK_IS_TYPE(TOKEN_TYPE_COLON)) {
                if (i == item->data.params - 1) {
                    ERROR = PARAMETERS_ERR;
                    return false;
                }
                continue;
            } else {
                ERROR = SYNTAX_ERR;
                return false;
            }
        }
    } else {
        GET_NEXT_TOKEN();
        if (TOK_IS_TYPE(TOKEN_TYPE_RIGHTB)) {
            if(!check_returns(var_type, item, var_num)) {
                return false;
            }
            return true;
        } else {
            ERROR = PARAMETERS_ERR;
            return false;
        }
    }
    if(!check_returns(var_type, item, var_num))
    {
        return false;
    }




    GET_NEXT_TOKEN();
    return true;
}

bool st_next_var_id(name_and_data *var_type, int *var_num) {
    if (TOK_IS_KW(KW_END) || TOK_IS_KW(KW_IF) || TOK_IS_KW(KW_WHILE) || TOK_IS_KW(KW_LOCAL) ||
        TOK_IS_KW(KW_RETURN) || TOK_IS_ID) {
        return true;
    }

    if (!TOK_IS_TYPE(TOKEN_TYPE_COLON)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    GET_NEXT_TOKEN();
    if (TOK_IS_ID) {
        if (isfunc(&scope, ID_NAME())) {
            if (!st_fnc_id(var_type, var_num)) {
                return false;
            }
            GET_NEXT_TOKEN();
        } else {
            if (expr(var_type, var_num)){
                if (*var_num > 0) {
                    ERROR = SEMANTIC_ERR;
                    return false;
                }
                return true;
            } else return false;
        }
    } else if (is_type_data()) {
        if (!expr(var_type, var_num)) {
            return false;
        } else if (*var_num > 0) {
            ERROR = SEMANTIC_ERR;
            return false;
        }
    } else {
        ERROR = SYNTAX_ERR;
        return false;
    }
    if (!st_next_var_id(var_type, var_num)) {
        return false;
    }
    return true;


}


bool st_var_id() {

    name_and_data var_type = NULL;
    int var_num = 0;


    if (!id_list(&var_type, &var_num)) {
        return false;
    }

    if (!TOK_IS_TYPE(TOKEN_TYPE_EQUAL)) {
        ERROR = SYNTAX_ERR;
    }

    GET_NEXT_TOKEN();
    if (TOK_IS_ID) {
        if (isfunc(&scope, ID_NAME())) {
            if (!st_fnc_id(&var_type, &var_num)) {
                return false;
            }
            GET_NEXT_TOKEN();
        } else {
            if (!expr(&var_type, &var_num)) {
                return false;
            } else if (var_num > 0) {
                ERROR = SEMANTIC_ERR;
                return false;
            }
        }
    } else if (is_type_data()) {
        if (!expr(&var_type, &var_num)) {
            return false;
        } else if (var_num > 0) {
            ERROR = SEMANTIC_ERR;
            return false;
        }
    } else {
        ERROR = SYNTAX_ERR;
        return false;
    }

    if (!st_next_var_id(&var_type, &var_num)) {
        return false;
    }
    return true;

}

bool exp_list(name_and_data *types, int * num) {
    GET_NEXT_TOKEN();
    if (!expr(types, num)) {
        return false;
    } else if (*num > 0) {
        ERROR = SEMANTIC_ERR;
        return false;
    }

    return next_exp(types, num);
}

bool next_exp(name_and_data *types, int * num) {

    //TODO added else to next_exp predict, is it ok? yep
    if (TOK_IS_KW(KW_IF) || TOK_IS_KW(KW_ELSE) || TOK_IS_KW(KW_WHILE) || TOK_IS_KW(KW_LOCAL) || TOK_IS_KW(KW_RETURN) ||
        TOK_IS_ID || TOK_IS_KW(KW_END))
        return true;

    if (!TOK_IS_TYPE(TOKEN_TYPE_COLON)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    GET_NEXT_TOKEN();
    if (!expr(types, num)) {
        return false;
    } else if (*num > 0) {
        ERROR = SEMANTIC_ERR;
        return false;
    }

    return next_exp(types,num);
}

bool option() {
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
bool expr(name_and_data *types, int * num) {
    //TODO dodělat podtyp nnumber a int

  /*  if(*types == NULL ){
        return true;
    }*/

    if(TOK_IS_KW(KW_END) ||
       TOK_IS_KW(KW_IF) || TOK_IS_KW(KW_WHILE) || TOK_IS_KW(KW_LOCAL) ||
       TOK_IS_KW(KW_RETURN) || TOK_IS_KW(KW_ELSE) || TOK_IS_KW(KW_THEN) || TOK_IS_KW(KW_GLOBAL) ||
       TOK_IS_KW(KW_FUNCTION) || TOK_IS_TYPE(TOKEN_TYPE_EOF)) {
        return true;
    } else if(TOK_IS_TYPE(TOKEN_TYPE_COLON)){
        GET_NEXT_TOKEN();
    }

    data_type_t typ;
    //generate variable and type
    ERROR = parse_expr(token, scope, &typ);

    if (ERROR == 0 && *types != NULL) {
        if(dyn_str_compare((*types)->string, "bool")){

            *num -= 1;
            return true;
        }
        if(typ == sym_data_to_data_type((*types)->datatype) || (typ == T_INT && (*types)->datatype == NUMBER)){
            *types = (*types)->next;

            *num -= 1;

            if(!expr(types, num)){
                return false;
            }

            return true;
        }
        ERROR = TYPE_INCOMPATIBILITY_ERR;
        return false;
    } else if (*types == NULL){
        //generate nil
        *num -= 1;
        if(!expr(types, num)){
            return false;
        }

        return true;
    }
    return false;
}

bool type_list(data_type *types, int *num) {
    GET_NEXT_TOKEN();
    if (TOK_IS_TYPE(TOKEN_TYPE_RIGHTB))
        return true;
    if (!is_type())
        return false;

    *types = create_data_type(get_datatype());
    (*num)++;
    if (next_type(types, num))
        return true;
    return false;
}

bool next_type(data_type *types, int *num) {
    GET_NEXT_TOKEN();
    if (TOK_IS_TYPE(TOKEN_TYPE_RIGHTB) || TOK_IS_KW(KW_GLOBAL) || TOK_IS_KW(KW_FUNCTION) || TOK_IS_KW(KW_END) ||
        TOK_IS_TYPE(TOKEN_TYPE_EOF) || TOK_IS_KW(KW_IF) || TOK_IS_KW(KW_WHILE) || TOK_IS_KW(KW_LOCAL) ||
        TOK_IS_KW(KW_RETURN) || TOK_IS_ID)
        return true;
    if (!TOK_IS_TYPE(TOKEN_TYPE_COLON)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    GET_NEXT_TOKEN();
    if (!is_type())
        return false;
    *types = add_data_type(*types, get_datatype());
    (*num)++;
    return next_type(types, num);

}

bool is_type() {
    if (TOK_IS_KW(KW_INTEGER) || TOK_IS_KW(KW_NUMBER) || TOK_IS_KW(KW_STRING))
        return true;
    ERROR = SYNTAX_ERR;
    return false;
}

bool is_type_data() {
    if (TOK_IS_TYPE(TOKEN_TYPE_INT) || TOK_IS_TYPE(TOKEN_TYPE_STR) || TOK_IS_TYPE(TOKEN_TYPE_DOUBLE))
        return true;
    ERROR = SYNTAX_ERR;
    return false;
}

sym_tab_datatype get_datatype() {
    if (TOK_IS_KW(KW_INTEGER))
        return INTEGER;
    else if (TOK_IS_KW(KW_NUMBER))
        return NUMBER;
    else if (TOK_IS_KW(KW_STRING))
        return STRING;

    return NIL;
}

sym_tab_datatype get_type_to_sym_type() {
    if (TOK_IS_TYPE(TOKEN_TYPE_INT))
        return INTEGER;
    else if (TOK_IS_TYPE(TOKEN_TYPE_DOUBLE))
        return NUMBER;
    else if (TOK_IS_TYPE(TOKEN_TYPE_STR))
        return STRING;

    return NIL;
}

// first id is check at call site
bool id_list(name_and_data *var_type, int *var_num) {
    if (!isvar(&scope, ID_NAME())) {
        ERROR = UNDEFINED_ERR;
        return false;
    }
    sym_tab_item_t *item = scope_search(&scope, ID_NAME());
    dynamic_string_t * name = (dynamic_string_t*)malloc(sizeof(dynamic_string_t));
    dyn_str_init(name);
    dyn_str_add_string(name, ID_NAME());
    *var_type = create_name_data(item->data.return_data_types->datatype, name);
    (*var_num)++;

    GET_NEXT_TOKEN();
    return next_id(var_type, var_num);
}

bool next_id(name_and_data *var_type, int *var_num) {
    if (TOK_IS_TYPE(TOKEN_TYPE_EQUAL))
        return true;
    if (!TOK_IS_TYPE(TOKEN_TYPE_COLON)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    GET_NEXT_TOKEN();
    if (!isvar(&scope, ID_NAME())) {
        ERROR = UNDEFINED_ERR;
        return false;
    }
    sym_tab_item_t *item = scope_search(&scope, ID_NAME());
    dynamic_string_t * name = (dynamic_string_t*)malloc(sizeof(dynamic_string_t));
    dyn_str_init(name);
    dyn_str_add_string(name, ID_NAME());
    *var_type = add_name_data(*var_type, item->data.return_data_types->datatype, name);
    (*var_num)++;

    GET_NEXT_TOKEN();
    return next_id(var_type, var_num);
}

//TODO překopat na zjišťování správných parametrů
bool term_list() {
    GET_NEXT_TOKEN();
    if (!is_term()) {
        return false;
    }
    if (!next_term())
        return false;

    return true;
}

bool next_term() {
    GET_NEXT_TOKEN();
    if (!TOK_IS_TYPE(TOKEN_TYPE_COLON)) {
        ERROR = SYNTAX_ERR;
        return false;
    }
    GET_NEXT_TOKEN();
    if (!is_term()) {
        return false;
    }
    if (!next_term())
        return false;

    return true;
}

bool is_term() {
    if (TOK_IS_TYPE(TOKEN_TYPE_STR) || TOK_IS_TYPE(TOKEN_TYPE_INT) || TOK_IS_TYPE(TOKEN_TYPE_DOUBLE) ||
        TOK_IS_KW(KW_NIL) || TOK_IS_ID) {
        return true;
    }
    ERROR = SYNTAX_ERR;
    return false;
}

data_type name_to_type(name_and_data nameAndData){
    data_type dataType = create_data_type(nameAndData->datatype);
    nameAndData = nameAndData->next;
    while(nameAndData != NULL){
        dataType = add_data_type(dataType, nameAndData->datatype);
        nameAndData = nameAndData->next;
    }
    return dataType;
}

data_type_t sym_data_to_data_type (sym_tab_datatype data){
    switch (data) {
        case INTEGER:
            return T_INT;
        case STRING:
            return T_STRING;
        case NUMBER:
            return T_NUMBER;
        case NIL:
            return T_NIL;
        default:
            return T_BOOL;
    }
}

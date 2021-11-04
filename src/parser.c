#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "scanner.h"
#include "dynamic_string.h"

#define TOK_IS_KW(kw) \
(token->type == TOKEN_TYPE_KW && token->attribute.keyword == (kw))

token_t *token;

bool body()
{
    get_next_token(token);
    if (token->type == TOKEN_TYPE_KW && token->attribute.keyword == KW_GLOBAL)
    {
        return false;
    }
    return true;
}


// <prog> -> require STRING <body> EOF
bool program()
{
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

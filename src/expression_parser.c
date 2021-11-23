//
// Created by martin on 23.11.21.
//
#include <stdlib.h>
#include <stdbool.h>
#include "expression_parser.h"

#include "scanner.h"
#include "parser.h"
#include "dynamic_string.h"
#include "stacks.h"

#define TOK_IS_OP() \
(token->type == TOKEN_TYPE_MINUS || token->type == TOKEN_TYPE_PLUS || token->type == TOKEN_TYPE_DIV_SIGN || token->type == TOKEN_TYPE_MUL_SIGN )


token_t* token

typedef enum {
    L,          //LEFT
    R,          //RIGHT
    E,          //EQUAL
    N,          //ERROR
    U           //UNDEFINED
} Pred_table_ops;

typedef enum {
    S_PLUS_MINUS,
    S_MUL_DIV_DOUBLEDIV,
    S_REL,
    S_HASH,
    S_DOUBLE_DOT,
    S_L_BRACKET,
    S_R_BRACKET,
    S_PIPE,
    S_DOLLAR
} Pred_table_cases;

int pred_table[9][9] = {
        {R, L, R, L, R, L, R, L, R},
        {R, R, R, L, R, L, R, L, R},
        {L, L, R, L, L, L, R, L, R},
        {R, R, R, U, R, L, U, L, R},
        {L, L, R, L, L, L, R, L, R},
        {L, L, L, U, L, L, E, L, N},
        {L, L, L, N, L, N, L, N, L},
        {L, L, L, N, L, N, L, N, L},
        {R, R, R, R, R, R, N, R, N}
};

Pred_table_symb get_case () {

    switch (token->type) {
        case TOKEN_TYPE_PLUS:
            return PLUS;
        case TOKEN_TYPE_MINUS:
            return MINUS;
        case TOKEN_TYPE_MUL_SIGN:
            return MUL;
        case TOKEN_TYPE_DIV_SIGN:
            return DIV;
        case TOKEN_TYPE_WN_DIV_SIGN:
            return DO;
        case TOKEN_TYPE_COMPARING:
            return EQ;
        case TOKEN_TYPE_COMPARING2:
            return NEQ;
        case TOKEN_TYPE_LESSEQ:
            return  LEQ;
        case TOKEN_TYPE_LESS:
            return LTN;
        case TOKEN_TYPE_GREATEREQ:
            return GEQ;
        case TOKEN_TYPE_GREATER:
            return GTN;
        case TOKEN_TYPE_LEFTB:
            return L_BRAC;
        case TOKEN_TYPE_RIGHTB:
            return R_BRAC;
        case TOKEN_TYPE_ID:
            return ID;
        case TOKEN_TYPE_INT:
            return INT;
        case TOKEN_TYPE_STR:
            return STRING;
        case TOKEN_TYPE_DOUBLE:
            return DOUBLE;
        default:
            return DOLLAR;


    }


}

void start() {
    Stack * op_stack;
    Stack_Init(op_stack);

    Stack * num_stack;
    Stack_Init(num_stack);
}


void pred_resolve_expression(int type, Stack * op_stack, Stack * num_stack) { //TODO type change to calling id type
    if(type == 0){  //INT
        if (token->type == TOKEN_TYPE_INT) {
            Stack_Push(num_stack, *token);
        } else if (TOK_IS_OP()) { /*TODO Check if top token on op_stack have precedence if not just push op if yes take last number and
            and commit op if one of two numbers is not id*/
            Stack_Push(op_stack, *token);
        }
    }

}



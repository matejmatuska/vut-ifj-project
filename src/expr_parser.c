#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "expr_parser.h"
#include "symbol_stack.h"

#include "scanner.h"
#include "ST_stack.h"
#include "error.h"
#include "symtable.h"
#include "code_gen.h"

#define ERR_IF_EITHER_NIL(type_a, type_b)           \
    while (0) {                                     \
        if ((type_a) == T_NIL || (type_b) == T_NIL) \
            return NIL_ERR;                         \
    }

#define RET_IF_ERR(func, ...)           \
    while (0) {                         \
        int _res = (func)(__VA_ARGS__); \
        if (_res != SYNTAX_OK)          \
        return _res;                    \
    }

ST_stack *ststack = NULL;

// parser operations
enum ops {
    S, // shift
    R, // reduce
    H, // shift with handle
    E, // error
};

// assigned values are indices to the precedence lookup table
typedef enum {
    PLUS_MINUS = 0,
    MUL_DIV_INTDIV = 1,
    REL = 2,
    LEN = 3,
    CONCAT = 4,
    L_PAREN = 5,
    R_PAREN = 6,
    VAL = 7,
    DOLLAR = 8
} lookup_index_t;

#define TABLE_SIZE 9

// parser operation lookup table
static enum ops op_lookup[TABLE_SIZE][TABLE_SIZE] = {
    { R, H, R, H, R, H, R, H, R },
    { R, R, R, H, R, H, R, H, R },
    { H, H, R, H, H, H, R, H, R },
    { R, R, R, H, R, H, R, H, R },
    { H, H, R, H, H, H, R, H, R },
    { H, H, H, H, H, H, S, H, E },
    { R, R, R, E, R, E, R, E, R },
    { R, R, R, E, R, E, R, E, R },
    { H, H, H, H, H, H, E, H, E }
};

#define IDX_PLUS_MINUS 0
#define IDX_MUL_DIT_IDIV 1
#define IDX_REL 2
#define IDX_LEN 3
#define IDX_CONCAT 4
#define IDX_L_PAREN 5
#define IDX_R_PAREN 6
#define IDX_I 7
#define IDX_DOLLAR 8

// returns symbol's index in the lookup table
static int get_lookup_index(sym_type_t type)
{
    switch (type)
    {
        case S_PLUS:
        case S_MINUS:
            return IDX_PLUS_MINUS;

        case S_MULT:
        case S_DIV:
        case S_INT_DIV:
            return IDX_MUL_DIT_IDIV;

        case S_LEQ:
        case S_LNE:
        case S_GEQ:
        case S_GNE:
        case S_EQ:
        case S_NEQ:
            return IDX_REL;

        case S_L_PAREN:
            return IDX_L_PAREN;

        case S_R_PAREN:
            return IDX_R_PAREN;

        case S_LEN:
            return IDX_LEN;

        case S_CONCAT:
            return IDX_CONCAT;

        case S_ID:
        case S_INT_LIT:
        case S_NUM_LIT:
        case S_STR_LIT:
        case S_NIL:
            return IDX_I;

        case S_DOLLAR:
            return IDX_DOLLAR;

        default:
            return IDX_DOLLAR;
    }
}

static enum ops lookup_operation(sym_type_t top_term, sym_type_t curr_sym)
{
    int top_index = get_lookup_index(top_term);
    int curr_index = get_lookup_index(curr_sym);
    return op_lookup[top_index][curr_index];
}

static sym_tab_datatype to_sym_datatype(data_type_t dt)
{
    switch (dt) {
        case T_INT:
            return INTEGER;
        case T_NUMBER:
            return NUMBER;
        case T_STRING:
            return STRING;
        case T_NIL:
            return NIL;
        default:
            return NIL;
    }
}

/**
 * Checks type compatibility of two operands
 *
 * @param a first operand's tyoe
 * @param b second operand's type
 * @return true if types are compatible else false
 */
static bool check_type_compat(data_type_t a, data_type_t b)
{
    if (a == b)
        return true;

    if ((a == T_INT && b == T_NUMBER) || (a == T_NUMBER && b == T_INT))
    {
        //TODO wait for code gen update
        generate_type_check_before_operation(to_sym_datatype(a), to_sym_datatype(b));
        return true;
    }

    return false;
}

static int rule_equality(data_type_t t1, data_type_t t2, data_type_t *res_type)
{
    if (!check_type_compat(t1, t2))
        if (!(t1 == T_NIL || t2 == T_NIL))
            return INCOMPATIBILITY_ERR;

    *res_type = T_BOOL;
    return SYNTAX_OK;
}

static int rule_relational(data_type_t t1, data_type_t t2, data_type_t *res_type)
{
    ERR_IF_EITHER_NIL(t1, t2);

    if (!check_type_compat(t1, t2))
        return INCOMPATIBILITY_ERR;

    *res_type = T_BOOL;
    return SYNTAX_OK;
}

static int rule_plus_minus_mult(data_type_t t1, data_type_t t2, data_type_t *res_type)
{
    ERR_IF_EITHER_NIL(t1, t2);

    if (!check_type_compat(t1, t2))
        return INCOMPATIBILITY_ERR;

    if (t1 == T_INT && t2 == T_INT)
        *res_type = T_INT;
    else
        *res_type = T_NUMBER;

    return SYNTAX_OK;
}

static int rule_divide(data_type_t t1, data_type_t t2, data_type_t *res_type)
{
    ERR_IF_EITHER_NIL(t1, t2);

    if (!check_type_compat(t1, t2))
            return INCOMPATIBILITY_ERR;

    *res_type = T_NUMBER;
    return SYNTAX_OK;
}

static int rule_int_divide(data_type_t t1, data_type_t t2, data_type_t *res_type)
{
    ERR_IF_EITHER_NIL(t1, t2);

    if (t1 != T_INT && t2 != T_INT)
        return INCOMPATIBILITY_ERR;

    *res_type = T_INT;
    return SYNTAX_OK;
}

static int rule_concat(data_type_t t1, data_type_t t2, data_type_t *res_type)
{
    ERR_IF_EITHER_NIL(t1, t2);

    if (t1 == T_STRING && t2 == T_STRING)
    {
        *res_type = T_STRING;
        return SYNTAX_OK;
    }

    return INCOMPATIBILITY_ERR;
}

/**
 * Reduces binary operator expression
 * @return error code
 */
static int apply_binary_rule(data_type_t t1, symbol_t operator,
        data_type_t t2, data_type_t *res_type)
{
    if (t1 == T_UNKNOWN || t2 == T_UNKNOWN)
        return UNDEFINED_ERR;

    switch (operator.type) {
        case S_PLUS:
            generate_operation(TOKEN_TYPE_PLUS);
            return rule_plus_minus_mult(t1, t2, res_type);
        case S_MINUS:
            generate_operation(TOKEN_TYPE_MINUS);
            return rule_plus_minus_mult(t1, t2, res_type);
        case S_MULT:
            generate_operation(TOKEN_TYPE_MUL_SIGN);
            return rule_plus_minus_mult(t1, t2, res_type);

        case S_DIV:
            generate_operation(TOKEN_TYPE_DIV_SIGN);
            return rule_divide(t1, t2, res_type);

        case S_INT_DIV:
            generate_operation(TOKEN_TYPE_WN_DIV_SIGN);
            return rule_int_divide(t1, t2, res_type);

        case S_CONCAT:
            generate_operation(TOKEN_TYPE_DOUBLE_DOT);
            return rule_concat(t1, t2, res_type);

        case S_EQ:
            generate_operation(TOKEN_TYPE_COMPARING);
            return rule_equality(t1, t2, res_type);
        case S_NEQ:
            generate_operation(TOKEN_TYPE_COMPARING2);
            return rule_equality(t1, t2, res_type);

        case S_LEQ:
            generate_operation(TOKEN_TYPE_LESSEQ);
            return rule_relational(t1, t2, res_type);
        case S_LNE:
            generate_operation(TOKEN_TYPE_LESS);
            return rule_relational(t1, t2, res_type);
        case S_GEQ:
            generate_operation(TOKEN_TYPE_GREATEREQ);
            return rule_relational(t1, t2, res_type);
        case S_GNE:
            generate_operation(TOKEN_TYPE_GREATER);
            return rule_relational(t1, t2, res_type);
        default:
            return SYNTAX_ERR;
    }
    return SYNTAX_OK;
}

static int rule_operand(data_type_t dtype, data_type_t *res_type)
{
    if (dtype == T_UNKNOWN)
        return UNDEFINED_ERR;

    *res_type = dtype;
    return SYNTAX_OK;
}

static int rule_length(data_type_t dtype, data_type_t *res_type)
{
    if (dtype != T_STRING)
        return UNDEFINED_ERR;

    //TODO generate code for string length
    *res_type = T_INT;
    return SYNTAX_OK;
}

/**
 * Applies a rule to a sequence of symbols on the top of the stack
 *
 * @note The derivation is reversed so the symbols are expected in the reverse order
 *
 * @param count number of symbols to apply rule to
 * @param symbols linked list containing count symbols
 *
 * @return error code
 */
static int apply_rule(int count, symbol_t *symbols, data_type_t *res_type)
{
    symbol_t *s1 = symbols;
    symbol_t *s2 = symbols ? symbols->next : NULL;
    symbol_t *s3 = s2 ? s2->next : NULL;

    if (count == 1) // operand
    {
        bool is_lit = s1->type == S_STR_LIT || s1->type == S_INT_LIT 
            || s1->type == S_NUM_LIT;

        if (is_lit || s1->type == S_ID || s1->type == S_NIL)
            return rule_operand(s1->data_type, res_type);
    }
    else if (count == 2) // unary operation
    {
        if (s1->type == S_NON_TERMINAL && s2->type == S_LEN)
        {
            generate_operation(TOKEN_TYPE_LENGTH);
            return rule_length(s1->data_type, res_type);
        }
    }
    else if (count == 3) // binary operation
    {
        // paren rule
        if (s1->type == S_R_PAREN
                && s2->type == S_NON_TERMINAL
                &&s3->type == S_L_PAREN)
        {
            if (s2->data_type == T_UNKNOWN)
                return INCOMPATIBILITY_ERR;

            *res_type = s2->data_type;
            return SYNTAX_OK;
        }

        if (s1->type == S_NON_TERMINAL && s3->type == S_NON_TERMINAL)
            return apply_binary_rule(s3->data_type, *s2, s1->data_type, res_type);
    }
    return SYNTAX_ERR;
}

/**
 * Converts token to symbol type
 */
static sym_type_t token_to_sym_type(token_t token)
{
    switch (token.type)
    {
        case TOKEN_TYPE_PLUS:
            return S_PLUS;
        case TOKEN_TYPE_MINUS:
            return S_MINUS;
        case TOKEN_TYPE_MUL_SIGN:
            return S_MULT;
        case TOKEN_TYPE_DIV_SIGN:
            return S_DIV;
        case TOKEN_TYPE_WN_DIV_SIGN:
            return S_INT_DIV;

        case TOKEN_TYPE_LENGTH:
            return S_LEN;
        case TOKEN_TYPE_DOUBLE_DOT:
            return S_CONCAT;

        case TOKEN_TYPE_COMPARING:
            return S_EQ;
        case TOKEN_TYPE_COMPARING2:
            return S_NEQ;
        case TOKEN_TYPE_LESSEQ:
            return S_LEQ;
        case TOKEN_TYPE_LESS:
            return S_LNE;
        case TOKEN_TYPE_GREATEREQ:
            return S_GEQ;
        case TOKEN_TYPE_GREATER:
            return S_GNE;

        case TOKEN_TYPE_LEFTB:
            return S_L_PAREN;
        case TOKEN_TYPE_RIGHTB:
            return S_R_PAREN;

        case TOKEN_TYPE_ID:
            return S_ID;

        case TOKEN_TYPE_INT:
            return S_INT_LIT;
        case TOKEN_TYPE_STR:
            return S_STR_LIT;
        case TOKEN_TYPE_DOUBLE:
        case TOKEN_TYPE_EXP:
        case TOKEN_TYPE_SIGN_EXP:
            return S_NUM_LIT;
        case TOKEN_TYPE_KW:
            if (token.attribute.keyword == KW_NIL)
                return S_NIL;
            return S_DOLLAR;

        default:
            return S_DOLLAR;
    }
}

/**
 * Returns data type of the token
 */
static data_type_t get_token_data_type(token_t token)
{
    if (token.type == TOKEN_TYPE_ID)
    {
        // adapt symtable api to our api
        sym_tab_key_t key = token.attribute.string->s;
        sym_tab_item_t *item = scope_search(&ststack, key);
        if (!item || !item->data.return_data_types)
            return T_UNKNOWN; // the variable is not defined

        // convert symtable data type to our
        sym_tab_datatype st_type = item->data.return_data_types->datatype;
        switch (st_type) {
            case INTEGER:
                return T_INT;
            case NUMBER:
                return T_NUMBER;
            case STRING:
                return T_STRING;
            case NIL:
                return T_NIL;
        }
    }

    switch (token.type) {
        case TOKEN_TYPE_INT:
            return T_INT;
        case TOKEN_TYPE_DOUBLE:
        case TOKEN_TYPE_EXP:
        case TOKEN_TYPE_SIGN_EXP:
            return T_NUMBER;
        case TOKEN_TYPE_STR:
            return T_STRING;
        case TOKEN_TYPE_KW:
            if (token.attribute.keyword == KW_NIL)
                return T_NIL;
            break;
        default:
            return T_UNKNOWN;
    }
    return T_UNKNOWN;
}

/**
 * Implementation of precedence analysis shift operation
 * Shifts symbol from input onto top of stack
 *
 * @return error code
 */
static int shift(sym_stack_t *stack, token_t *token,
        sym_type_t sym, data_type_t dtype)
{
    if (!sym_stack_push(stack, sym, dtype))
        return INTERNAL_ERR;

    if (get_next_token(token) == LEX_ERR)
        return LEX_ERR;

    return SYNTAX_OK;
}

/**
 * Implementation of precedence analysis shift with handle operation
 * Inserts "handle" after top terminal on the stack and
 * shifts symbol from input onto top of stack
 *
 * @return error code
 */
static int shift_w_handle(sym_stack_t *stack, token_t *token,
        sym_type_t sym, data_type_t dtype)
{
    if (!sym_stack_insert_handle(stack))
        return INTERNAL_ERR;

    if (sym == S_INT_LIT || sym == S_STR_LIT || sym == S_NUM_LIT || sym == S_ID || sym == S_NIL)
    {
        generate_push(token);
    }

    shift(stack, token, sym, dtype);

    return SYNTAX_OK;
}

/**
 * Implementation of precedence analysis reduce operation
 * Reduces the part of expression from the top of the symbol stack
 * to the special symbol "handle" by trying to apply one of set rules
 *
 * @param stack the symbol stack
 * @return error code
 */
static int reduce(sym_stack_t *stack)
{
    int count = 0;
    symbol_t *to_reduce = sym_stack_top_to_handle(stack, &count);

    data_type_t data_type = T_UNKNOWN;
    int res = apply_rule(count, to_reduce, &data_type);
    if (res != SYNTAX_OK)
        return res;

    // pop all reduced symbols + handle
    for (int i = 0; i < count + 1; i++)
        sym_stack_pop(stack);

    sym_stack_push(stack, S_NON_TERMINAL, data_type);
    return SYNTAX_OK;
}

/**
 * Looks ahead one token and determines if current token (invalid)
 * isn't part of next expression, then returns the token back to the scanner
 *
 * For example:
 * local a : integer = 5 + 13
 * a = a + 42
 *
 * @return error code
 */
static int check_end_of_expr()
{
    token_t *next = malloc(sizeof(token_t));
    token_init(next);

    if (get_next_token(next) == LEX_ERR)
        return LEX_ERR;

    if (next->type == TOKEN_TYPE_COLON
            || next->type == TOKEN_TYPE_EQUAL
            || next->type == TOKEN_TYPE_LEFTB)
    {
        return_token(next);
        return SYNTAX_OK;
    }
    //TODO free token in case of error
    return SYNTAX_ERR;
}

/**
 * Parses expression starting at token
 * If an error occurs the parser determines if it's an actual error
 * and not one caused by false end of expression detection
 * by peeking at the next token
 *
 * @param token the first token of the expression
 * @param stack symbol stack used by precedence analysis
 * @return error code
 */
static int analyze(token_t *token, sym_stack_t *stack, data_type_t *res_type)
{
    if (!sym_stack_push(stack, S_DOLLAR, T_UNKNOWN))
        return INTERNAL_ERR;

    symbol_t *top_term = sym_stack_top_terminal(stack);
    sym_type_t curr_sym = token_to_sym_type(*token);

    bool detected_end = false;
    while (!(curr_sym == S_DOLLAR && top_term->type == S_DOLLAR))
    {
        int result = SYNTAX_ERR;
        data_type_t data_type = get_token_data_type(*token);
        switch (lookup_operation(top_term->type, curr_sym))
        {
            case S: // shift
                result = shift(stack, token, curr_sym, data_type);
                if (result != SYNTAX_OK)
                    return result;
                break;

            case H: // shift with handle
                result = shift_w_handle(stack, token, curr_sym, data_type);
                if (result != SYNTAX_OK)
                    return result;
                break;

            case R: // reduce
                result = reduce(stack);
                if (result != SYNTAX_OK)
                    return result;
                break;

            case E: // error
                result = check_end_of_expr();
                if (result != SYNTAX_OK)
                    return result;

                detected_end = true; // we found end, force DOLLAR symbol
                break;
        }
        top_term = sym_stack_top_terminal(stack);
        curr_sym = detected_end ? S_DOLLAR : token_to_sym_type(*token);
    }

    *res_type = sym_stack_top(stack)->data_type;
    return SYNTAX_OK;
}

int parse_expr(token_t *token, ST_stack *st_stack, data_type_t *res_type)
{
    ststack = st_stack;

    sym_stack_t stack;
    sym_stack_init(&stack);

    int result = analyze(token, &stack, res_type);

    sym_stack_free(&stack);
    return result;
}

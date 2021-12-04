#include <stdlib.h>
#include <stdbool.h>

#include "expr_parser.h"
#include "symbol_stack.h"

#include "scanner.h"
#include "ST_stack.h"
#include "error.h"
#include "symtable.h"

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

// returns symbol's index in the lookup table
static int get_lookup_index(symbol_type_t type)
{
    switch (type)
    {
        case S_PLUS:
        case S_MINUS:
            return PLUS_MINUS;

        case S_MULT:
        case S_DIV:
        case S_INT_DIV:
            return MUL_DIV_INTDIV;

        case S_LEQ:
        case S_LNE:
        case S_GEQ:
        case S_GNE:
        case S_EQ:
        case S_NEQ:
            return REL;

        case S_L_PAREN:
            return L_PAREN;

        case S_R_PAREN:
            return R_PAREN;

        case S_LEN:
            return LEN;

        case S_CONCAT:
            return CONCAT;

        case S_ID:
        case S_INT_LIT:
        case S_NUM_LIT:
        case S_STR_LIT:
        case S_NIL:
            return VAL;

        case S_DOLLAR:
            return DOLLAR;

        default:
            return DOLLAR;
    }
}

static enum ops get_operation(symbol_type_t top_terminal, symbol_type_t current_sym)
{
    return op_lookup[get_lookup_index(top_terminal)][get_lookup_index(current_sym)];
}

/**
 * Checks type compatibility of two operands
 *
 * @param a first operand's tyoe
 * @param b second operand's type
 * @return true if types are compatible else false
 */
bool check_type_compat(data_type_t a, data_type_t b)
{
    if (a == b)
        return true;

    if ((a == T_INT && b == T_NUMBER) || (a == T_NUMBER && b == T_INT))
        return true;

    return false;
}

/**
 * Reduces binary operator expression
 * @return error code
 */
static int reduce_binary(symbol_t op1, symbol_t operator,
        symbol_t op2, data_type_t *res_type)
{
    if (op1.data_type == T_NIL || op2.data_type == T_NIL)
        if (operator.type != S_EQ && operator.type != S_NEQ)
            return NIL_ERR;

    switch (operator.type) {
        case S_PLUS:
        case S_MINUS:
        case S_MULT:
            // plus minus multiply rules
            if (!check_type_compat(op1.data_type, op2.data_type))
                return INCOMPATIBILITY_ERR;

            if (op1.data_type == T_INT && op2.data_type == T_INT)
                *res_type = T_INT;
            else
                *res_type = T_NUMBER;
            break;

        case S_DIV:
            // division rule
            if (!check_type_compat(op1.data_type, op2.data_type))
                    return INCOMPATIBILITY_ERR;

            *res_type = T_NUMBER;
            break;

        case S_INT_DIV:
            // integer division rule
            if (op1.data_type != T_INT && op2.data_type != T_INT)
                return INCOMPATIBILITY_ERR;

            *res_type = T_INT;
            break;

        case S_CONCAT:
            // concatenation rule
            if (op1.data_type == T_STRING && op2.data_type == T_STRING)
                *res_type = T_STRING;
            else
                return INCOMPATIBILITY_ERR;
            break;

        case S_LEQ:
        case S_LNE:
        case S_GEQ:
        case S_GNE:
        case S_EQ:
        case S_NEQ:
            // relational operator rule
            if (!check_type_compat(op1.data_type, op2.data_type))
                return INCOMPATIBILITY_ERR;

            *res_type = T_BOOL;
            break;

        default:
            return SYNTAX_ERR;
    }
    return SYNTAX_OK;
}

/**
 * Applies a rule to a sequence of symbols
 *
 * @note The derivation is reversed so the symbols are expected in the reverse order
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

    switch (count)
    {
        case 1:
            // operand rule
            {
                bool is_lit = s1->type == S_STR_LIT || s1->type == S_INT_LIT 
                    || s1->type == S_NUM_LIT;

                if (is_lit || s1->type == S_ID || s1->type == S_NIL) {
                    if (s1->data_type == T_UNKNOWN)
                        return UNDEFINED_ERR;

                    //TODO generate code for operand
                    *res_type = s1->data_type;
                    return SYNTAX_OK;
                }
                return SYNTAX_ERR;
            }
        case 2:
            // length rule
            if (s1->type == S_NON_TERMINAL && s2->type == S_LEN)
            {
                if (s1->data_type != T_STRING)
                    return UNDEFINED_ERR;

                //TODO generate code for string length
                *res_type = s1->data_type;
                return SYNTAX_OK;
            }
            return SYNTAX_ERR;

        case 3:
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
            {
                if (s1->data_type == T_UNKNOWN || s3->data_type == T_UNKNOWN)
                    return UNDEFINED_ERR;

                return reduce_binary(*s3, *s2, *s1, res_type);
            }
            return SYNTAX_ERR;

        default:
            return SYNTAX_ERR;
    }
}

/**
 * Converts token to symbol type
 */
symbol_type_t token_to_sym_type(token_t token)
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
data_type_t get_token_data_type(token_t token)
{
    if (token.type == TOKEN_TYPE_ID)
    {
        // adapt symtable api to our api
        sym_tab_key_t key = token.attribute.string->s;
        sym_tab_item_t *item = scope_search(&ststack, key);
        if (!item)
            return T_UNKNOWN; // the variable is not in defined

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
            return T_NUMBER;
        case TOKEN_TYPE_STR:
            return T_STRING;
        case TOKEN_TYPE_KW:
            if (token.attribute.keyword == KW_NIL)
                return T_NIL;
            else
                return T_UNKNOWN;
        default:
            return T_UNKNOWN;
    }
    return T_UNKNOWN;
}

/**
 * Reduces the part of expression from the top of the symbol stack
 * to the special token "handle" by trying to apply one of set rules
 *
 * @param stack the symbol stack
 * @return error code
 */
static int reduce(symbol_stack_t *stack)
{
    int count = 0;
    symbol_t *to_reduce = symbol_stack_top_to_handle(stack, &count);

    if (count < 1 || count > 3)
        return SYNTAX_ERR;

    data_type_t data_type = T_UNKNOWN;
    int res = apply_rule(count, to_reduce, &data_type);
    if (res != SYNTAX_OK)
        return res;

    // pop all reduced symbols + handle
    for (int i = 0; i < count + 1; i++)
        symbol_stack_pop(stack);

    symbol_stack_push(stack, S_NON_TERMINAL, data_type);
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
int analyze(token_t *token, symbol_stack_t *stack, data_type_t *res_type)
{
    if (!symbol_stack_push(stack, S_DOLLAR, T_UNKNOWN))
        return INTERNAL_ERR;

    symbol_t *top_term = symbol_stack_top_terminal(stack);
    symbol_type_t curr_sym = token_to_sym_type(*token);

    while (!(curr_sym == S_DOLLAR && top_term->type == S_DOLLAR))
    {
        data_type_t data_type = get_token_data_type(*token);
        switch (get_operation(top_term->type, curr_sym))
        {
            case S: // shift
                if (!symbol_stack_push(stack, curr_sym, data_type))
                    return INTERNAL_ERR;
                else
                    if(get_next_token(token) == LEX_ERR)
                        return LEX_ERR;
                break;

            case H: // shift with handle
                if (!symbol_stack_insert_handle(stack))
                    return INTERNAL_ERR;

                if (!symbol_stack_push(stack, curr_sym, data_type))
                    return INTERNAL_ERR;

                if(get_next_token(token) == LEX_ERR)
                    return LEX_ERR;

                break;

            case R: // reduce
                {
                    int result = reduce(stack);
                    if (result != SYNTAX_OK)
                        return result;
                    break;
                }
            case E: // error
                {
                    int result = check_end_of_expr();
                    if (result != SYNTAX_OK)
                        return result;

                    *res_type = symbol_stack_top(stack)->data_type;
                    return SYNTAX_OK;
                }
        }
        top_term = symbol_stack_top_terminal(stack);
        curr_sym = token_to_sym_type(*token);
    }

    *res_type = symbol_stack_top(stack)->data_type;
    return SYNTAX_OK;
}

int parse_expr(token_t *token, ST_stack *st_stack, data_type_t *res_type)
{
    ststack = st_stack;

    symbol_stack_t stack;
    symbol_stack_init(&stack);

    int result = analyze(token, &stack, res_type);

    symbol_stack_free(&stack);
    return result;
}

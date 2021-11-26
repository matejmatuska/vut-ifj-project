#include <stdlib.h>
#include <stdbool.h>

#include "scanner.h"
#include "expr_parser.h"
#include "symbol_stack.h"
#include "error.h"

// parser operations
typedef enum {
    S, // shift
    R, // reduce
    H, // shift with handle
    E, // error
    T // TODO
} ops_t;

// assigned values are indices to the precedence lookup table
typedef enum {
    S_PLUS_MINUS = 0,
    S_MUL_DIV_INTDIV = 1,
    S_REL = 2,
    S_LEN = 3,
    S_CONCAT = 4,
    S_L_PAREN = 5,
    S_R_PAREN = 6,
    S_VAL = 7,
    S_DOLLAR = 8
} lookup_index_t;

#define TABLE_SIZE 9

// parser operation lookup table
static ops_t op_lookup[TABLE_SIZE][TABLE_SIZE] = {
    { R, H, R, H, R, H, R, H, R },
    { R, R, R, H, R, H, R, H, R },
    { H, H, R, H, H, H, R, H, R },
    { R, R, R, T, R, H, T, H, R },
    { H, H, R, H, H, H, R, H, R },
    { H, H, H, T, H, H, S, H, E },
    { R, R, R, E, R, E, R, E, R },
    { R, R, R, E, R, E, R, E, R },
    { H, H, H, H, H, H, E, H, E }
};

// parser grammar rules
typedef enum {
    NO_MATCH,  // symbol sequence on stack doesn't match any rule

    E_PLUS_E,  // E -> E + E
    E_MINUS_E, // E -> E - E
    E_MUL_E,   // E -> E* E
    E_DIV_E,   // E -> E / E
    E_INT_DIV_E, // E -> E // E

    E_EQ_E,  // E -> E == E
    E_NEQ_E, // E -> E ~= E
    E_LEQ_E, // E -> E <= E
    E_LNE_E, // E -> E < E
    E_GEQ_E, // E -> E >= E
    E_GNE_E, // E -> E > E

    LEN_E, // E -> #E
    E_CONCAT_E, // E -> E .. E

    LP_E_RP, // E -> (E)

    VAL_TO_E, // E -> i
} rule_t;

// returns symbol's index in the lookup table
static int get_lookup_index(symbol_type_t type)
{
    switch (type)
    {
        case PLUS:
        case MINUS:
            return S_PLUS_MINUS;

        case MULT:
        case DIV:
        case INT_DIV:
            return S_MUL_DIV_INTDIV;

        case LEQ:
        case LNE:
        case GEQ:
        case GNE:
        case EQ:
        case NEQ:
            return S_REL;

        case L_PAREN:
            return S_L_PAREN;

        case R_PAREN:
            return S_R_PAREN;

        case LEN:
            return S_LEN;

        case CONCAT:
            return S_CONCAT;

        case ID:
        case INT_LIT:
        case NUM_LIT:
        case STR_LIT:
        case NIL:
            return S_VAL;

        case DOLLAR:
            return S_DOLLAR;

        default:
            return S_DOLLAR;
    }
}

static ops_t get_operation(symbol_type_t top_terminal, symbol_type_t current_sym)
{
    return op_lookup[get_lookup_index(top_terminal)][get_lookup_index(current_sym)];
}

/**
 * Matches a sequence of symbols to a rule
 *
 * @note The derivation is reversed so the symbols are expected in the reverse order
 * @param count number of symbols to match rule to
 * @param symbols linked list containing count symbols
 * @param rule destination to save rule to
 *
 * @return true symbols match some rule, otherwise false
 */
static int match_rule(int count, symbol_t *symbols, rule_t *rule)
{
    symbol_t *s1 = symbols;
    symbol_t *s2 = symbols ? symbols->next : NULL;
    symbol_t *s3 = s2 ? s2->next : NULL;

    switch (count)
    {
        case 1:
            if (s1->type == ID || s1->type == STR_LIT
                    || s1->type == INT_LIT || s1->type == NUM_LIT
                    || s1->type == NIL)
            {
                return VAL_TO_E; // E -> i
            }
            return NO_MATCH;

        case 2:
            if (s1->type == NON_TERMINAL && s2->type == LEN)
                return LEN_E;

            return NO_MATCH;

        case 3:
            if (s1->type == R_PAREN && s2->type == NON_TERMINAL && s3->type == L_PAREN)
                return LP_E_RP;

            if (s1->type == NON_TERMINAL && s3->type == NON_TERMINAL)
                switch (s2->type)
                {
                    case PLUS:
                        return E_PLUS_E;
                    case MINUS:
                        return E_MINUS_E;
                    case MULT:
                        return E_MUL_E;
                    case DIV:
                        return E_DIV_E;
                    case INT_DIV:
                        return E_INT_DIV_E;

                    case CONCAT:
                        return E_CONCAT_E;

                    case EQ:
                        return E_EQ_E;
                    case NEQ:
                        return E_NEQ_E;

                    case LEQ:
                        return E_LEQ_E;
                    case LNE:
                        return E_LNE_E;
                    case GEQ:
                        return E_GEQ_E;
                    case GNE:
                        return E_GNE_E;

                    default:
                        return NO_MATCH;
                }

        default:
            return NO_MATCH;
    }

}

/**
 * Converts token to symbol
 */
symbol_type_t token_to_symbol(token_t token)
{
    switch (token.type)
    {
        case TOKEN_TYPE_PLUS:
            return PLUS;
        case TOKEN_TYPE_MINUS:
            return MINUS;
        case TOKEN_TYPE_MUL_SIGN:
            return MULT;
        case TOKEN_TYPE_DIV_SIGN:
            return DIV;
        case TOKEN_TYPE_WN_DIV_SIGN:
            return INT_DIV;

        case TOKEN_TYPE_LENGTH:
            return LEN;
        case TOKEN_TYPE_DOUBLE_DOT:
            return CONCAT;

        case TOKEN_TYPE_COMPARING:
            return EQ;
        case TOKEN_TYPE_COMPARING2:
            return NEQ;
        case TOKEN_TYPE_LESSEQ:
            return LEQ;
        case TOKEN_TYPE_LESS:
            return LNE;
        case TOKEN_TYPE_GREATEREQ:
            return GEQ;
        case TOKEN_TYPE_GREATER:
            return GNE;

        case TOKEN_TYPE_LEFTB:
            return L_PAREN;
        case TOKEN_TYPE_RIGHTB:
            return R_PAREN;

        case TOKEN_TYPE_ID:
            return ID;

        case TOKEN_TYPE_INT:
            return INT_LIT;
        case TOKEN_TYPE_STR:
            return STR_LIT;
        case TOKEN_TYPE_DOUBLE:
            return NUM_LIT;
        case TOKEN_TYPE_KW:
            if (token.attribute.keyword == KW_NIL)
                return NIL;
            return DOLLAR;

        default:
            return DOLLAR;
    }
}

static int reduce(symbol_stack_t *stack)
{
    int count = 0;
    symbol_t *to_reduce = symbol_stack_top_to_handle(stack, &count);

    if (count < 1 || count > 3)
        return SYNTAX_ERR;

    rule_t rule;
    if (match_rule(count, to_reduce, &rule) == NO_MATCH)
    {
        return SYNTAX_ERR;
    }

    // pop all reduced symbols + handle
    for (int i = 0; i < count + 1; i++)
        symbol_stack_pop(stack);

    symbol_stack_push(stack, NON_TERMINAL);
    return SYNTAX_OK;
}

int analyze(token_t *token, symbol_stack_t *stack)
{
    if (!symbol_stack_push(stack, DOLLAR))
        return INTERNAL_ERR;

    symbol_t *top_terminal;
    symbol_type_t current_sym;

    top_terminal = symbol_stack_top_terminal(stack);
    current_sym = token_to_symbol(*token);

    while (!(current_sym == DOLLAR && top_terminal->type == DOLLAR))
    {
        switch (get_operation(top_terminal->type, current_sym))
        {
            case S: // shift
                if (!symbol_stack_push(stack, current_sym))
                    return INTERNAL_ERR;
                else
                    if(get_next_token(token) == LEX_ERR)
                        return LEX_ERR;
                break;

            case H: // shift with handle
                if (!symbol_stack_insert_handle(stack))
                    return INTERNAL_ERR;

                if (!symbol_stack_push(stack, current_sym))
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
                return SYNTAX_ERR;
        }
        top_terminal = symbol_stack_top_terminal(stack);
        current_sym = token_to_symbol(*token);
    }
    return SYNTAX_OK;
}

/**
 * Runs the expression parser
 *
 * @return error code
 */
int parse_expr(token_t *token)
{
    symbol_stack_t stack;
    symbol_stack_init(&stack);

    int result = analyze(token, &stack);

    symbol_stack_free(&stack);
    return result;
}

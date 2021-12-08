#include <stdlib.h>
#include <stdbool.h>

#include "sym_stack.h"

static bool is_empty(const sym_stack_t *stack)
{
    return stack->top == NULL;
}

// inserts handle after top terminal
bool sym_stack_insert_handle(sym_stack_t *stack)
{
    if (is_empty(stack))
        return false;

    // if the top symbol is non-terminal just push the handle
    if (stack->top && stack->top->type != S_NON_TERMINAL)
    {
        sym_stack_push(stack, S_HANDLE, T_UNKNOWN);
        return true;
    }

    // else find the first non-terminal
    symbol_t *iter = stack->top;
    while (iter && iter->type == S_NON_TERMINAL)
    {
        // if next symbol is terminal insert handle
        symbol_t *next = iter->next;
        if (next && next->type != S_NON_TERMINAL)
        {
            symbol_t *handle = malloc(sizeof(symbol_t));
            if (!handle)
                return false;

            handle->type = S_HANDLE;

            iter->next = handle;
            handle->next =next;
            return true;
        }
        // if not just continue
        iter = iter->next;
    }

    // no top terminal
    return false;
}

symbol_t *sym_stack_top_to_handle(sym_stack_t *stack, int *count)
{
    *count = 0;

    symbol_t *iter = stack->top;
    while (iter && iter->type != S_HANDLE)
    {
        (*count)++;
        iter = iter->next;
    }
    return stack->top;
}

symbol_t *sym_stack_top_terminal(sym_stack_t *stack)
{
    symbol_t *iter = stack->top;
    while(iter && iter->type == S_NON_TERMINAL)
    {
        iter = iter->next;
    }
    return iter;
}

void sym_stack_init(sym_stack_t *stack)
{
    stack->top = NULL;
}

bool sym_stack_push(sym_stack_t *stack, sym_type_t sym, data_type_t dtype)
{
    symbol_t *new = malloc(sizeof(symbol_t));
    if (!new)
    {
        return false;
    }
    new->type = sym;
    new->data_type = dtype;
    new->next = stack->top;
    stack->top = new;
    return true;
}

symbol_t *sym_stack_top(sym_stack_t *stack)
{
    if (is_empty(stack))
        return NULL;

    return stack->top;
}

bool sym_stack_pop(sym_stack_t *stack)
{
    if (is_empty(stack))
        return false;

    symbol_t *tmp = stack->top;
    stack->top = tmp->next;
    free(tmp);
    return true;
}

void sym_stack_free(sym_stack_t *stack)
{
    symbol_t *iter = stack->top;
    while (iter != NULL)
    {
        symbol_t *tmp = iter->next;
        free(iter);
        iter = tmp;
    }
}

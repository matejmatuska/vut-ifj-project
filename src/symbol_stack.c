#include <stdlib.h>
#include <stdbool.h>

#include "symbol_stack.h"

static bool is_empty(const symbol_stack_t *stack)
{
    return stack->top == NULL;
}

// inserts handle after top terminal
bool symbol_stack_insert_handle(symbol_stack_t *stack)
{
    if (is_empty(stack))
    {
        //TODO error
        return false;
    }

    // if the top symbol is non-terminal just push the handle
    if (stack->top && stack->top->type != NON_TERMINAL)
    {
        symbol_stack_push(stack, HANDLE);
        return true;
    }

    // else find the first non-terminal
    symbol_t *iter = stack->top;
    while (iter && iter->type == NON_TERMINAL)
    {
        // if next symbol is terminal insert handle
        symbol_t *next = iter->next;
        if (next && next->type != NON_TERMINAL)
        {
            symbol_t *handle = malloc(sizeof(symbol_t));
            if (!handle)
            {
                //TODO
                return false;
            }
            handle->type = HANDLE;

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

typedef struct {
    symbol_t *head;
    int len;
} symbol_list_t;

symbol_t *symbol_stack_top_to_handle(symbol_stack_t *stack, int *count)
{
    *count = 0;

    symbol_t *iter = stack->top;
    while (iter && iter->type != HANDLE)
    {
        (*count)++;
        iter = iter->next;
    }
    return stack->top;
}

symbol_t *symbol_stack_top_terminal(symbol_stack_t *stack)
{
    symbol_t *iter = stack->top;
    while(iter && iter->type == NON_TERMINAL)
    {
        iter = iter->next;
    }
    return iter;
}

void symbol_stack_init(symbol_stack_t *stack)
{
    stack->top = NULL;
}

bool symbol_stack_push(symbol_stack_t *stack, symbol_type_t sym)
{
    symbol_t *new = malloc(sizeof(symbol_t));
    if (!new)
    {
        return false;
    }
    new->type = sym;
    new->next = stack->top;
    stack->top = new;
    return true;
}

symbol_t *symbol_stack_top(symbol_stack_t *stack)
{
    if (is_empty(stack))
        return NULL;

    return stack->top;
}

bool symbol_stack_pop(symbol_stack_t *stack)
{
    if (is_empty(stack))
        return false;

    symbol_t *tmp = stack->top;
    stack->top = tmp->next;
    free(tmp);
    return true;
}

void symbol_stack_free(symbol_stack_t *stack)
{
    symbol_t *iter = stack->top;
    while (iter != NULL)
    {
        symbol_t *tmp = iter->next;
        free(iter);
        iter = tmp;
    }
}

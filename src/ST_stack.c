/*
Implementace překladače imperativního jazyka IFJ21
Júlia Mazáková, xmazak02
*/
#include "ST_stack.h"
#include "error.h"

/**
  * Acces to the top of ST_stack
  * @param  ST_stack *scope -> current scope's visibility
  * @return top symbol table
*/
sym_tab_t *top_table(ST_stack *stack)
{
    if (stack->top != NULL)
    {
        return stack->top->localtable;
    }
    else
        return NULL;
}
/**
  * Push another local table into the stack
  * @param  ST_stack **scope - current scope's visibility
  * @return true if success
*/
bool push(struct ST_stack *stack)
{
    stack->level++;
    st_stack_item_t *new = malloc(sizeof(st_stack_item_t));

    (stack->uid)++; // increase the uid to keep it u(nique)
    sym_tab_t *table = sym_tab_init(stack->uid, stack->level);

    // check if stack was allocated correctly
    if (!new)
    {
        return false;
    }
    // if we are pushing first symbol table (aka global)
    new->localtable = table;
    new->next = stack->top;
    stack->top = new;
    return true;
}

/**
  * Pop the top of ST_stack
  * @param  ST_stack *scope - current scope's visibility
  * @return bool if success
*/
bool pop(ST_stack *stack)
{
    st_stack_item_t *tmp;
    // check if scope is empty
    if (stack->top == NULL)
    {
        return false;
    }
    tmp = stack->top;
    stack->top = stack->top->next;
    sym_tab_free(tmp->localtable);
    free(tmp);

    stack->level--;
    return true;
}
/**
  * Search for item everywhere (local -> global)
  * @param  ST_stack **scope - current scope's visibility
  * @param  sym_tab_key_t key - key we want to find
  * @return found item or NULL if item doesn't exist
*/
sym_tab_item_t *scope_search(ST_stack *stack, sym_tab_key_t key, size_t *uid, size_t *level)
{
    st_stack_item_t *top = stack->top;
    // if symbol table is empty
    if (top == NULL)
    {
        return NULL;
    }
    else
    {
        st_stack_item_t *temp = top;
        while (temp != NULL)
        {
            sym_tab_item_t *tmp = NULL;
            if ((tmp = sym_tab_find_in_table(temp->localtable, key)))
            {
                // found the id
                *uid = temp->localtable->uid;
                *level = temp->localtable->nest_level;
                return tmp;
            }
            temp = temp->next;
        }
        return NULL;
    }
}

/**
  * Initialize stack
  * @return initialized stack
*/
void init_ST_stack(ST_stack *stack)
{
    stack->top = NULL;
    stack->level = 0;
    stack->uid = 0;
}

/**
  * Free all remaining stacks
  * @param ST_stack **scope -> current scope's visibilty
  * @return true if succesfuly freed
*/
bool free_ST_stack(ST_stack *stack)
{
    stack->level = 0;
    stack->uid = 0;
    st_stack_item_t *top = stack->top;
    // check for empty symbol table
    if (top == NULL)
    {
        return false;
    }
    else
    {
        // loop through symtables
        while (top != NULL)
        {
            st_stack_item_t *tmp = top;
            top = tmp->next;
            sym_tab_free(tmp->localtable);
            free(tmp);
        }
        stack->top = NULL;
        return true;
    }
    return false;
}
/**
  * Find out if item is function
  * @param  sym_tab_t *t - current symbol table
  * @param  sym_tab_key_t key - key we want to check
  * @return true if it's function
*/
bool isfunc(ST_stack *stack, sym_tab_key_t key)
{
    size_t uid;
    size_t level;

    sym_tab_item_t *item = scope_search(stack, key,&uid, &level);
    if (item == NULL)
        return false;
    else
    {
        if (item->data.item_type == HT_FUNC)
            return true;
        else
            return false;
    }
}
/**
  * Find out if item is a variable
  * @param  sym_tab_t *t - current symbol table
  * @param  sym_tab_key_t key - key we want to check
  * @bool  - true if it's variable
*/
bool isvar(ST_stack *stack, sym_tab_key_t key)
{
    size_t uid;
    size_t level;

    sym_tab_item_t *item = scope_search(stack, key,&uid, &level);
    if (item == NULL)
        return false;
    else
    {
        if (item->data.item_type == HT_VAR)
            return true;
        else
            return false;
    }
}

size_t st_stack_level(ST_stack *stack)
{
    return stack->level;
}

size_t st_stack_uid(ST_stack *stack)
{
    return stack->uid;
}

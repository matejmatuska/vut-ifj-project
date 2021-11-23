//
// Created by martin on 23.11.21.
//

#include "stacks.h"

int STACK_SIZE = MAX_STACK;
int error_flag;


void Stack_Error(int error_code)
{
    static const char *SERR_STRINGS[MAX_SERR + 1] = {
            "Unknown error",
            "Stack error: INIT",
            "Stack error: PUSH",
            "Stack error: TOP"};

    if (error_code <= 0 || error_code > MAX_SERR)
        error_code = 0;
    printf("%s\n", SERR_STRINGS[error_code]);
    error_flag = 1;
}

void Stack_Init(Stack *stack)
{
    //Kontrola prázdného pointeru
    if (stack == NULL)
    {
        Stack_Error(SERR_INIT);
        return;
    }
    //Index vrací -1 jako neplatný index v array
    stack->topIndex = -1;
}

int Stack_IsEmpty(const Stack *stack)
{
    //Pokud je index nastaven na -1 je to neplatný array index a je prázdný
    if (stack->topIndex == -1)
        return 1;
    return 0;
}

int Stack_IsFull(const Stack *stack)
{
    //Kontroluje zda top index není stejně velký jako maximální velikost
    if (stack->topIndex == STACK_SIZE - 1)
        return 1;
    return 0;
}

void Stack_Top(const Stack *stack, token_t *dataPtr)
{
    if (Stack_IsEmpty(stack) != 0)
    {
        Stack_Error(SERR_TOP);
        return;
    }

    *dataPtr = stack->array[stack->topIndex];
}

void Stack_Pop(Stack *stack)
{
    if (Stack_IsEmpty(stack) != 0)
    {
        return;
    }
    stack->topIndex = stack->topIndex - 1;
}


void Stack_Push(Stack *stack, token_t data)
{
    if (Stack_IsFull(stack) != 0)
    {
        Stack_Error(SERR_PUSH);
        return;
    }

    stack->topIndex = stack->topIndex + 1;
    stack->array[stack->topIndex] = data;
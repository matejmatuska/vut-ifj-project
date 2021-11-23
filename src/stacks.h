//
// Created by martin on 23.11.21.
//

#ifndef TACKS_H
#define TACKS_H

#include "scanner.h"

int MAX_STACK = 50;

typedef struct {
    token_t array[MAX_STACK];
    int topIndex;
} Stack;

void Stack_Error( int );

void Stack_Init( Stack * );

int Stack_IsEmpty( const Stack * );

int Stack_IsFull( const Stack * );

void Stack_Top( const Stack *, char * );

void Stack_Pop( Stack * );

void Stack_Push( Stack *, char );

#endif

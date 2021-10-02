#include "parser.h"

#include <stdio.h>
#include <string.h>

//struct that stores all important informations about token //TODO
typedef struct {
    char name [16];
    char type [16];


} token;

//Works as linked list (mmaybe as both ways in future) which will be putted to stack and sent to generator -> will go by
//importance in rules (will do computation tree)
typedef struct {
    int* head;
    unsigned int index;
    token *first;
    token *op;
    token *second;

    int* next;
} expr;


//-_-
void hello(void) {
    printf("Hello, World!\n");
}

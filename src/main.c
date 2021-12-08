/*
Implementace překladače imperativního jazyka IFJ21
Martin Pokorný, xpokor85
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "scanner.h"
#include "parser.h"


int main() {
    get_source(stdin);
    return parse();
}

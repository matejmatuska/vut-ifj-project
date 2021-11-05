#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "scanner.h"
#include "parser.h"

int main() {

    get_source(stdin);

    if (!program())
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

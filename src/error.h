#ifndef ERROR_H
#define ERROR_H

typedef enum
{
    SYNTAX_OK = 0,
    LEX_ERR = 1,
    SYNTAX_ERR = 2,
    UNDEFINED_ERR = 3,
    TYPE_INCOMPATIBILITY_ERR = 4,
    PARAMETERS_ERR = 5,
    INCOMPATIBILITY_ERR = 6,
    SEMANTIC_ERR = 7,
    NIL_ERR = 8,
    DIV_BY_ZERO_ERR = 9,
    INTERNAL_ERR = 99
} Error;

#endif // ERROR_H

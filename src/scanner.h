#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include "dynamic_string.h"
#include "error.h"

typedef enum
{
  TOKEN_TYPE_ID,          // identificator
  TOKEN_TYPE_KW,          // keyword
  TOKEN_TYPE_STR,         // string
  TOKEN_TYPE_INT,         // 3
  TOKEN_TYPE_DOUBLE,      // 3.14
  TOKEN_TYPE_EXP,         // e^14
  TOKEN_TYPE_SIGN_EXP,    // e^-15
  TOKEN_TYPE_PLUS,        // +
  TOKEN_TYPE_MINUS,       // -
  TOKEN_TYPE_MUL_SIGN,    // *
  TOKEN_TYPE_DIV_SIGN,    // /
  TOKEN_TYPE_WN_DIV_SIGN, // //
  TOKEN_TYPE_COMPARING,   // ==
  TOKEN_TYPE_COMPARING2,  // ~=
  TOKEN_TYPE_LESS,        // <
  TOKEN_TYPE_LESSEQ,      // <=
  TOKEN_TYPE_GREATER,     // >
  TOKEN_TYPE_GREATEREQ,   // >=
  TOKEN_TYPE_EQUAL,       // =
  TOKEN_TYPE_LENGTH,      // #
  TOKEN_TYPE_DEF,         // :
  TOKEN_TYPE_LEFTB,       // (
  TOKEN_TYPE_RIGHTB,       // )
  TOKEN_TYPE_COLON,        // ,
  TOKEN_TYPE_DOUBLE_DOT,	//..
  TOKEN_TYPE_EOF

} token_type;

typedef enum
{
  KW_DO,
  KW_ELSE,
  KW_END,
  KW_FUNCTION,
  KW_GLOBAL,
  KW_IF,
  KW_INTEGER,
  KW_NUMBER,
  KW_STRING,
  KW_LOCAL,
  KW_NIL,
  KW_REQUIRE,
  KW_RETURN,
  KW_THEN,
  KW_WHILE
} keyword;

typedef union token_attribute
{
  int integer_value;
  double double_value;
  dynamic_string_t *string;
  keyword keyword;
} token_attribute_t;

typedef struct token
{
  token_type type;
  union token_attribute attribute;
} token_t;

int get_next_token(token_t* token);
void return_token(token_t* token);

void token_init(token_t* token);
void token_free(token_t* token);
void set_string(dynamic_string_t* string);
void get_source(FILE* file);
#endif //SCANNER_H

#include "scanner.h"
#include "error.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

FILE *source;

// all possible states of Final State Machine

#define START_STATE 300 //f
#define MINUS_STATE 301 //F
#define START_OF_LINE_COMMENT_STATE 302
#define POTENTIAL_BLOCK_COMMENT_STATE 303
#define BLOCK_COMMENT_STATE 304
#define POTENTIAL_END_OF_BLOCK_COMMENT_STATE 305
#define LINE_COMMENT_STATE 306
#define LINE_COMMENT_STATE_2 307
#define ID_STATE 308  //F
#define INT_STATE 309 //F
#define START_OF_DOUBLE_STATE 310
#define DOUBLE_STATE 311 //F
#define START_OF_EXP_STATE 312
#define START_OF_SIGN_EXP_STATE 313
#define EXP_STATE 314        //F
#define EXP_W_SIGN_STATE 315 //F
#define START_OF_STRING_STATE 316
#define START_OF_ESCAPE_SEQ_STATE 317
#define ASCII_SECOND_VALUE_STATE 318
#define ASCII_THIRD_VALUE_STATE 319
#define STRING_STATE 320 //F
#define MUL_STATE 321    //F
#define DIV_STATE 322    //F
#define START_OF_COMPARING_STATE 323
#define LESS_STATE 324          //F
#define GREATER_STATE 325       //F
#define LENGTH_STATE 326        //F
#define DEF_STATE 327           //F
#define LEFT_BRACKET_STATE 328  //F
#define RIGHT_BRACKET_STATE 329 //F
#define PLUS_STATE 330          //F
#define LESSEQ_STATE 331        //F
#define GREATEREQ_STATE 332     //F
#define COMPARING_2_STATE 333   //F
#define WN_DIV_STATE 334        //F
#define EQUAL_STATE 335         //F
#define COMPARING_STATE 336     //F
#define EOF_STATE 337           //F
#define COLON_STATE 338         //F
#define DOT_STATE 339           
#define DOUBLE_DOT_STATE 340     //F

// error MACRO
#define error(type, value) \
    dyn_str_free(value);\
    return type;


/**
  *Set source
* @param FILE *file -> specific file or stdin
  * @return nothing
*/
void get_source(FILE *file)
{
    source = file;
}
/**
  *Initialization of token
* @param token_t *token -> token to be initialized
  * @return nothing
*/
void token_init(token_t* token)
{
    token->type = TOKEN_TYPE_INT;
    token->attribute.integer_value = 0;
}
/**
  *Free function for token
* @param token_t *token -> token to be freed
  * @return nothing
*/
void token_free(token_t* token)
{
    //dyn_str_free if the token type is ID or STRING
    if (token->type == TOKEN_TYPE_ID || token->type == TOKEN_TYPE_STR)
    {
        dyn_str_free(token->attribute.string);
        token->attribute.integer_value = 0;
    }
    else
    {
        token->attribute.integer_value = 0;
    }
    token->type = TOKEN_TYPE_INT;
}
/**
  *Function for making number from dynamic string
* @param token_t *token -> current token
* @param dynamic_string_t *value -> possible number
  * @return 0 if correct
*/
int make_number(token_t *token, dynamic_string_t *value)
{
    if (token->type == TOKEN_TYPE_INT)
        token->attribute.integer_value = atoi(value->s);
    else
        token->attribute.double_value = atof(value->s);

    dyn_str_free(value);
    return 0;
}
/**
  *Function for making string from dynamic string
* @param token_t *token -> current token
* @param dynamic_string_t *value -> possible string
  * @return 0 if correct and 1 if incorrect
*/
int make_string(token_t *token, dynamic_string_t *value)
{

    for (size_t i = 0; i < value->size; i++)
    {
        if (value->s[i] == '\\')
        {
            if (value->s[i + 1] == '\\')
            {
                dyn_str_del_character(value, i);
                value->s[i] = '\\';
            }
            else if (value->s[i + 1] == 'n')
            {
                dyn_str_del_character(value, i);
                value->s[i] = '\n';
            }
            else if (value->s[i + 1] == 't')
            {
                dyn_str_del_character(value, i);
                value->s[i] = '\t';
            }
            else if (value->s[i + 1] == '"')
            {
                dyn_str_del_character(value, i);
                value->s[i] = '"';
            }
            else if (isdigit(value->s[i + 1]))
            {
                char substring[4];
                memcpy(substring, &(value->s)[i + 1], 3);
                substring[3] = '\0';
                int ascii = atoi(substring);
                if (ascii < 1 || ascii>255)
                {
                    return 1;
                }
                dyn_str_del_character(value, i);
                dyn_str_del_character(value, i);
                dyn_str_del_character(value, i);
                value->s[i] = ascii;
            }
        
        }
    }
        token->attribute.string = value;
        
    return 0;
}
/**
  *Function for making id or keyword from dynamic string
* @param token_t *token -> current token
* @param dynamic_string_t *value -> possible id or kw
  * @return 0 if correct
*/
int make_id_or_kw(token_t *token, dynamic_string_t *value)
{
    
    if (dyn_str_compare(value, "do"))
    {
        token->type = TOKEN_TYPE_KW;
        token->attribute.keyword = KW_DO;
        
    }
    else if (dyn_str_compare(value, "else"))
    {
        token->type = TOKEN_TYPE_KW;
        token->attribute.keyword = KW_ELSE;
        
    }
    else if (dyn_str_compare(value, "end"))
    {
        token->type = TOKEN_TYPE_KW;
        token->attribute.keyword = KW_END;
        
    }
    else if (dyn_str_compare(value, "function"))
    {
        token->type = TOKEN_TYPE_KW;
        token->attribute.keyword = KW_FUNCTION;
        
    }
    else if (dyn_str_compare(value, "global"))
    {
        token->type = TOKEN_TYPE_KW;
        token->attribute.keyword = KW_GLOBAL;
        
    }
    else if (dyn_str_compare(value, "if"))
    {
        token->type = TOKEN_TYPE_KW;
        token->attribute.keyword = KW_IF;
        
    }
    else if (dyn_str_compare(value, "integer"))
    {
        token->type = TOKEN_TYPE_KW;
        token->attribute.keyword = KW_INTEGER;
        
    }
    else if (dyn_str_compare(value, "number"))
    {
        token->type = TOKEN_TYPE_KW;
        token->attribute.keyword = KW_NUMBER;
        
    }
    else if (dyn_str_compare(value, "string"))
    {
        token->type = TOKEN_TYPE_KW;
        token->attribute.keyword = KW_STRING;
        
    }
    else if (dyn_str_compare(value, "local"))
    {
        token->type = TOKEN_TYPE_KW;
        token->attribute.keyword = KW_LOCAL;
        
    }
    else if (dyn_str_compare(value, "nil"))
    {
        token->type = TOKEN_TYPE_KW;
        token->attribute.keyword = KW_NIL;
        
    }
    else if (dyn_str_compare(value, "require"))
    {
        token->type = TOKEN_TYPE_KW;
        token->attribute.keyword = KW_REQUIRE;
        
    }
    else if (dyn_str_compare(value, "return"))
    {
        token->type = TOKEN_TYPE_KW;
        token->attribute.keyword = KW_RETURN;
        
    }
    else if (dyn_str_compare(value, "then"))
    {
        token->type = TOKEN_TYPE_KW;
        token->attribute.keyword = KW_THEN;
        
    }
    else if (dyn_str_compare(value, "while"))
    {
        token->type = TOKEN_TYPE_KW;
        token->attribute.keyword = KW_WHILE;
        
    }
    else
    {
        token->attribute.string = value;
        return 0;
    }
    dyn_str_free(value);
    return 0;
}

/**
 *Function for obtaining next token with all his parameters
* @param token_t *token -> current token
 * @return 0 if correct , otherwise return value of LEX_ERROR
*/
int get_next_token(token_t* current_token)
{

    //checking file
    if (source == NULL)
        return INTERNAL_ERR;
     
    token_free(current_token);

    dynamic_string_t* value = NULL;
    
    char c;
    // initial state
    int state = 300;
    while (true)
    { //reading char by char

        c = getc(source);
        switch (state)
        {
        case START_STATE:
            if (c == ',')
                state = COLON_STATE;
            else if (c == '.')
                state = DOT_STATE;
            else if (c == '-')
                state = MINUS_STATE;
            else if (c == EOF)
                state = EOF_STATE;
            else if (isspace(c))
                state = START_STATE;
            else if (c == '"')
            {
                state = START_OF_STRING_STATE;
                value = (dynamic_string_t*)malloc(sizeof(dynamic_string_t));
                if (value == NULL)
                {
                    fprintf(stderr, "Allocation error\n");
                    return INTERNAL_ERR;
                }
                if (!dyn_str_init(value))
                {
                    error(LEX_ERR, value)
                }
            }
            else if (c == '+')
                state = PLUS_STATE;
            else if (c == '*')
                state = MUL_STATE;
            else if (c == '/')
                state = DIV_STATE;
            else if (c == '~')
                state = START_OF_COMPARING_STATE;
            else if (c == '<')
                state = LESS_STATE;
            else if (c == '>')
                state = GREATER_STATE;
            else if (c == '=')
                state = EQUAL_STATE;
            else if (c == '#')
                state = LENGTH_STATE;
            else if (c == ':')
                state = DEF_STATE;
            else if (c == '(')
                state = LEFT_BRACKET_STATE;
            else if (c == ')')
                state = RIGHT_BRACKET_STATE;
            else if ((('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z')) || (c == '_'))
            {
                state = ID_STATE;
                value = (dynamic_string_t*)malloc(sizeof(dynamic_string_t));
                if (value == NULL)
                {
                    fprintf(stderr, "Allocation error\n");
                    return INTERNAL_ERR;
                }
                if (!dyn_str_init(value))
                {
                    error(LEX_ERR, value)
                }

                dyn_str_add_character(value, c);
            }
            else if (('0' <= c) && (c <= '9'))
            {
                state = INT_STATE;
                value = (dynamic_string_t*)malloc(sizeof(dynamic_string_t));
                if (value == NULL)
                {
                    fprintf(stderr, "Allocation error\n");
                    return INTERNAL_ERR;
                }
                if (!dyn_str_init(value))
                {
                    error(LEX_ERR, value)
                }

                dyn_str_add_character(value, c);
            }
            else
            {
                error(LEX_ERR, value)
            }
            break;

        case MINUS_STATE:
            if (c == '-')
                state = START_OF_LINE_COMMENT_STATE;
            else
            {
                current_token->type = TOKEN_TYPE_MINUS;
                ungetc(c,source);
                return 0;
            }
            break;

        case START_OF_LINE_COMMENT_STATE:
            if (c == '[')
                state = POTENTIAL_BLOCK_COMMENT_STATE;
            else if (c == '\n')
                state = START_STATE;
            else
                state = LINE_COMMENT_STATE;
            break;

        case POTENTIAL_BLOCK_COMMENT_STATE:
            if (c == '[')
                state = BLOCK_COMMENT_STATE;
            else if (c == '\n')
                state = START_STATE;
            else
                state = LINE_COMMENT_STATE;
            break;

        case BLOCK_COMMENT_STATE:
            if (c == ']')
                state = POTENTIAL_END_OF_BLOCK_COMMENT_STATE;
            else if (c == '\n' || c == EOF)
                state = START_STATE;
            else
                state = BLOCK_COMMENT_STATE;
            break;

        case POTENTIAL_END_OF_BLOCK_COMMENT_STATE:
            if (c == ']' || c == '\n' || c == EOF)
                state = START_STATE;
            else
                state = BLOCK_COMMENT_STATE;
            break;

        case LINE_COMMENT_STATE:
            if (c == '\n' || (c == EOF))
                state = START_STATE;
            else
                state = LINE_COMMENT_STATE;
            break;

        case ID_STATE:
            if ((('0' <= c) && (c <= '9')) || (('a' <= c) && (c <= 'z')) || (('A' <= c) && (c <= 'Z')) || (c == '_'))
            {
                dyn_str_add_character(value, c);
                state = ID_STATE;
            }
            else
            {
                current_token->type = TOKEN_TYPE_ID;
                ungetc(c, source);
                make_id_or_kw(current_token, value);
                return 0;
            }
            break;

        case INT_STATE:
            if (('0' <= c) && (c <= '9'))
            {
                dyn_str_add_character(value, c);
                state = INT_STATE;
            }
            else if (c == '.')
            {
                dyn_str_add_character(value, c);
                state = START_OF_DOUBLE_STATE;
            }
            else if ((c == 'e') || (c == 'E'))
            {
                dyn_str_add_character(value, c);
                state = START_OF_EXP_STATE;
            }
            else
            {
                current_token->type = TOKEN_TYPE_INT;
                ungetc(c, source);
                make_number(current_token, value);
                return 0;
            }
            break;

        case START_OF_DOUBLE_STATE:
            if (('0' <= c) && (c <= '9'))
            {
                dyn_str_add_character(value, c);
                state = DOUBLE_STATE;
            }
            else
            {
                error(LEX_ERR, value)
                return 0;
            }
            break;

        case DOUBLE_STATE:
            if (('0' <= c) && (c <= '9'))
            {
                dyn_str_add_character(value, c);
                state = DOUBLE_STATE;
            }
            else if ((c == 'e') || (c == 'E'))
            {
                dyn_str_add_character(value, c);
                state = START_OF_EXP_STATE;
            }
            else
            {
                current_token->type = TOKEN_TYPE_DOUBLE;
                ungetc(c, source);
                make_number(current_token, value);
                return 0;
            }
            break;
        case START_OF_EXP_STATE:
            if (('0' <= c) && (c <= '9'))
            {
                dyn_str_add_character(value, c);
                state = EXP_STATE;
            }
            else if ((c == '-') || (c == '+'))
            {
                dyn_str_add_character(value, c);
                state = START_OF_SIGN_EXP_STATE;
            }
            else
            {
                error(LEX_ERR, value)
            }
            break;

        case START_OF_SIGN_EXP_STATE:
            if (('0' <= c) && (c <= '9'))
            {
                dyn_str_add_character(value, c);
                state = EXP_W_SIGN_STATE;
            }
            else
            {
                error(LEX_ERR, value)
            }
            break;

        case EXP_STATE:
            if (('0' <= c) && (c <= '9'))
            {
                dyn_str_add_character(value, c);
                state = EXP_STATE;
            }
            else
            {
                current_token->type = TOKEN_TYPE_EXP;
                ungetc(c, source);
                make_number(current_token, value);
                return 0;
            }
            break;

        case EXP_W_SIGN_STATE:
            if (('0' <= c) && (c <= '9'))
            {
                dyn_str_add_character(value, c);
                state = EXP_W_SIGN_STATE;
            }
            else
            {
                current_token->type = TOKEN_TYPE_SIGN_EXP;
                ungetc(c, source);
                make_number(current_token, value);
                return 0;
            }
            break;

        case START_OF_STRING_STATE:
            if (c == '"')
                state = STRING_STATE;
            else if (c == '\\')
            {
                dyn_str_add_character(value, c);
                state = START_OF_ESCAPE_SEQ_STATE;
            }
            else if (c < 31)
            {
                error(LEX_ERR, value)
            }
            else
            {
                dyn_str_add_character(value, c);
                state = START_OF_STRING_STATE;
            }
            break;

        case START_OF_ESCAPE_SEQ_STATE:
            if ((c == '\\') || (c == 'n') || (c == 't')|| (c == '"'))
            {
                dyn_str_add_character(value, c);
                state = START_OF_STRING_STATE;
            }
            else if (('0' <= c) && (c <= '2'))
            {
                dyn_str_add_character(value, c);
                state = ASCII_SECOND_VALUE_STATE;
            }
            else
            {
                error(LEX_ERR, value)
            }
            break;

        case ASCII_SECOND_VALUE_STATE:
            if (('0' <= c) && (c <= '9'))
            {
                dyn_str_add_character(value, c);
                state = ASCII_THIRD_VALUE_STATE;
            }
            else
            {
                error(LEX_ERR, value)
            }
            break;

        case ASCII_THIRD_VALUE_STATE:
            if (('0' <= c) && (c <= '9'))
            {
                dyn_str_add_character(value, c);
                state = START_OF_STRING_STATE;
            }
            else
            {
                error(LEX_ERR, value)
            }
            break;

        case STRING_STATE:
           
            if(make_string(current_token, value)==0)
            {
                current_token->type = TOKEN_TYPE_STR;
                ungetc(c,source);
                return 0;
            }
            else
            {
                error(LEX_ERR,value)
            }
            break;

        case MUL_STATE:
            current_token->type = TOKEN_TYPE_MUL_SIGN;
            ungetc(c,source);
           return 0;
            break;

        case DIV_STATE:
            if (c == '/')
                state = WN_DIV_STATE;
            else
            {
                current_token->type = TOKEN_TYPE_DIV_SIGN;
                ungetc(c,source);
                    return 0;
            }
            break;

        case START_OF_COMPARING_STATE:
            if (c == '=')
                state = COMPARING_2_STATE;
            else
            {
                error(LEX_ERR, value)
            }
            break;

        case LESS_STATE:
            if (c == '=')
                state = LESSEQ_STATE;
            else
            {
                current_token->type = TOKEN_TYPE_LESS;
                ungetc(c,source);
                    return 0;
            }
            break;

        case GREATER_STATE:
            if (c == '=')
                state = GREATEREQ_STATE;
            else
            {
                current_token->type = TOKEN_TYPE_GREATER;
                ungetc(c,source);
                    return 0;
            }
            break;

        case LENGTH_STATE:
            current_token->type = TOKEN_TYPE_LENGTH;
            ungetc(c,source);
                return 0;
            break;

        case DEF_STATE:
            current_token->type = TOKEN_TYPE_DEF;
            ungetc(c,source);
                return 0;
            break;

        case LEFT_BRACKET_STATE:
            current_token->type = TOKEN_TYPE_LEFTB;
            ungetc(c,source);
                return 0;
            break;

        case RIGHT_BRACKET_STATE:
            current_token->type = TOKEN_TYPE_RIGHTB;
            ungetc(c,source);
                return 0;
            break;

        case PLUS_STATE:
            current_token->type = TOKEN_TYPE_PLUS;
            ungetc(c,source);
                return 0;
            break;

        case LESSEQ_STATE:
            current_token->type = TOKEN_TYPE_LESSEQ;
            ungetc(c,source);
                return 0;
            break;

        case GREATEREQ_STATE:
            current_token->type = TOKEN_TYPE_GREATEREQ;
            ungetc(c,source);
                return 0;
            break;

        case COMPARING_2_STATE:
            current_token->type = TOKEN_TYPE_COMPARING2;
            ungetc(c,source);
                return 0;
            break;

        case WN_DIV_STATE:
            current_token->type = TOKEN_TYPE_WN_DIV_SIGN;
            ungetc(c,source);
                return 0;
            break;

        case EQUAL_STATE:
            if (c == '=')
                state = COMPARING_STATE;
            else
            {
                current_token->type = TOKEN_TYPE_EQUAL;
                ungetc(c,source);
                    return 0;
            }
            break;

        case COMPARING_STATE:
            current_token->type = TOKEN_TYPE_COMPARING;
                return 0;
            break;

        case COLON_STATE:
            current_token->type = TOKEN_TYPE_COLON;
            return 0;
            break;

        case DOT_STATE:
            if (c == '.')
            {
                state = DOUBLE_DOT_STATE;              
            }
            else
            {
                error(LEX_ERR, value)
            }
            break;

        case DOUBLE_DOT_STATE:
            current_token->type = TOKEN_TYPE_DOUBLE_DOT;
            ungetc(c, source);
            return 0;
            break;

        case EOF_STATE:
            current_token->type = TOKEN_TYPE_EOF;
                return 0;
            break;
        }
  }
   
}

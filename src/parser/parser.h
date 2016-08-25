//1.Ok
#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include <stdio.h>
#include "../tree.h"
#include "../types.h"
#include "../extends.h"
#include "../tree.h"
#include "../stack.h"
#include "../Array.h"
#include "../Id.h"


#define PARSER_BUFFER_SIZE 8

typedef struct
{
    char         end_of_data;
    char        *source;
    char       (*get_byte)(char *source);
    char       (*end_data)(char *source);
    char        *buffer;
    unsigned int buffer_length,
                 buffered_length,
                 buffered_symbol_pos;

    char         head;
    Stack       *blocks_pos;
    Stack       *scopes;
    Tree        *cur_scope;
    Array       *cur_body;
    Data        *cur_block;
    Function    *cur_function;
    Id          *id;
    String      *expr_token;
    String      *token;
    char         is_end_function;

    Array       *sources;
    Tree        *system_libraries;

    Array       *stack_infered_types;
}
Parser;


void init_parser();
Function* run_parser(char *source, char (*get_byte)(Parser *parser), char (*end_of_data)(Parser *parser));


#endif // PARSER_H_INCLUDED

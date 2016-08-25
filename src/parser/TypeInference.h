#ifndef TYPEINFERENCE_H_INCLUDED
#define TYPEINFERENCE_H_INCLUDED

#include "parser.h"

typedef struct
{
    Array *body;
    Array *arguments;
}
FunctionBody;

char infer_type(Parser *parser, Array *expression);
Array* infer_function(Function *function, Array *arguments);
void print_infered_type(char type);

#endif // TYPEINFERENCE_H_INCLUDED

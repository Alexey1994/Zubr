#ifndef VARIABLE_H_INCLUDED
#define VARIABLE_H_INCLUDED

#include "parser.h"


Variable* find_variable(String *name, Parser *parser);
Variable* new_static_variable(Parser *parser, String *name, char type, int *data);
Variable* new_variable(Parser *parser, String *name, char type);
char parser_get_variable(Parser *parser, String *variable_name, char type);

#endif // VARIABLE_H_INCLUDED

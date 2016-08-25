#ifndef EXPRESSION_H_INCLUDED
#define EXPRESSION_H_INCLUDED

#include "parser.h"

void init_expression();

Array* parser_get_expression(Parser *parser);
Array* parser_get_condition(Parser *parser);

#endif // EXPRESSION_H_INCLUDED

#ifndef OPERAND_H_INCLUDED
#define OPERAND_H_INCLUDED

#include "parser.h"

char get_operand(Parser *parser, Array *expression);
Array* get_compound_operand(Parser *parser);

#endif // OPERAND_H_INCLUDED

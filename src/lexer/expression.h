#ifndef EXPRESSION_H_INCLUDED
#define EXPRESSION_H_INCLUDED

#include "lexer_operations.h"

List* lexer_get_expression(Lexer *lexer);
List* lexer_get_condition(Lexer *lexer);

#endif // EXPRESSION_H_INCLUDED

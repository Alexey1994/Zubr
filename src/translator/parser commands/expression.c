#include "expression.h"
#include "../types.h"


char translator_parser_command_expression(TranslatorParser *parser)
{
    Data *expression_data;

    array_push(parser->current_state, new_data(expression_data, TRANSLATOR_EXPRESSION));
}

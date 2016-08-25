#include "break.h"


char parser_break(Parser *parser)
{
    array_push(parser->cur_body, new_data(0, BREAK));
    return 1;
}

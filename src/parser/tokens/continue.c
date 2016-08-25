#include "continue.h"


char parser_continue(Parser *parser)
{
    array_push(parser->cur_body, new_data(0, CONTINUE));
    return 1;
}

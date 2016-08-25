#include "loop.h"


char parser_loop(Parser *parser)
{
    Array *loop_body=array_init(8);

    add_block(parser, loop_body, LOOP, loop_body);

    return 1;
}

#include "break.h"
#include "../debug.h"


char parser_break(Parser *parser)
{
    if(debug)
    {
        print_block();
        printf("BREAK\n");
    }

    array_push(parser->cur_body, new_data(0, BREAK));
    return 1;
}

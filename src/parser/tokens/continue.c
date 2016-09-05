#include "continue.h"
#include "../debug.h"


char parser_continue(Parser *parser)
{
    if(debug)
    {
        print_block();
        printf("CONTINUE\n");
    }

    array_push(parser->cur_body, new_data(0, CONTINUE));
    return 1;
}

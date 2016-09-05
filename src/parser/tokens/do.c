#include "do.h"
#include "../debug.h"


char parser_do(Parser *parser)
{
    if(debug)
    {
        print_block();
        printf("DO\n");
        block_pos++;
    }

    Do *do_data;

    do_data=new(Do);
    do_data->body=array_init(8);

    add_block(parser, do_data, DO, do_data->body);

    return 1;
}

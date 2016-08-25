#include "do.h"


char parser_do(Parser *parser)
{
    Do    *do_data;

    do_data=new(Do);
    do_data->body=array_init(8);

    add_block(parser, do_data, DO, do_data->body);

    return 1;
}

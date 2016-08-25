#include "include.h"


static char include_program(Parser *parser)
{
    String *include_name=get_new_token(parser);
    FILE   *f=fopen(include_name->begin, "rb");

    if(!f)
    {
        printf("\nфайл ");
        str_print(include_name);
        printf(" не найден");

        return 0;
    }

    array_push(parser->sources, parser->source);

    parser->source=f;
    //parser->read_byte(parser);

    return 1;
}

char parser_include(Parser *parser)
{
    if(!include_program(parser))
        return 0;

    while(parser->head==',')
    {
        read_byte(parser);

        if(!include_program(parser))
            return 0;

        skip(parser);
    }

    return 1;
}

#include "../translator.h"
#include "out.h"
#include "../types.h"


char* get_string_translator_parser(TranslatorParser *parser)
{
    String *string=str_init("");
    char   *returned_string;

    get_byte_translator_parser(parser);

    while(parser->head!='\'' && !parser->end_data)
    {
        if(parser->head=='\\')
        {
            get_byte_translator_parser(parser);

            switch(parser->head)
            {
                case 'n':  str_push(string, '\n'); break;
                case 'r':  str_push(string, '\r'); break;
                case 't':  str_push(string, '\t'); break;
                case '\\': str_push(string, '\\'); break;
                case '\'': str_push(string, '\''); break;
            }
        }
        else
            str_push(string, parser->head);

        get_byte_translator_parser(parser);
    }

    if(parser->head!='\'')
    {
        printf("отсутствует закрывающая '\n");
        return 0;
    }

    get_byte_translator_parser(parser);

    returned_string=string->begin;
    free(string);
    return returned_string;
}


char translator_parser_command_out(TranslatorParser *parser)
{
    char *string;

    skip_translator_parser(parser);

    if(parser->head=='\'')
    {
        string=get_string_translator_parser(parser);

        if(!string)
            return 0;

        array_push(parser->current_state, new_data(string, TRANSLATOR_OUT));
        //out_token_translator_parser(parser, string);
    }
}

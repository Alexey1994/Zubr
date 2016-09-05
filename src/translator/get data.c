#include "get data.h"
#include "../character.h"


void get_byte_translator_parser(TranslatorParser *translator)
{
    translator->head=translator->get_byte(translator->in_source);

    if(translator->end_of_data(translator->in_source))
        translator->end_data=1;
}


void skip_translator_parser(TranslatorParser *parser)
{
    while(is_space(parser->head))
        get_byte_translator_parser(parser);
}


char* get_new_token_translator_parser(TranslatorParser *parser)
{
    String *token=str_init("");
    char   *returned_token;

    skip_translator_parser(parser);

    while(is_letter(parser->head) || parser->head=='_')
    {
        str_push(token, parser->head);
        get_byte_translator_parser(parser);
    }

    if(!token->length)
    {
        str_free(token);
        return 0;
    }

    returned_token=token->begin;
    free(token);

    return returned_token;
}


char* get_token_translator_parser(TranslatorParser *parser)
{
    char   *returned_token;

    str_clear(parser->token);
    skip_translator_parser(parser);

    while(is_letter(parser->head) || parser->head=='_')
    {
        str_push(parser->token, parser->head);
        get_byte_translator_parser(parser);
    }

    if(!parser->token->length)
        return 0;

    returned_token=parser->token->begin;

    return returned_token;
}


void out_token_translator_parser(TranslatorParser *parser, char *token)
{
    for(; *token; token++)
        parser->out_byte(*token, parser->out_source);
}

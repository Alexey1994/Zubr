#include "get data.h"
#include "../character.h"


void get_byte_translator_parser(TranslatorParser *translator)
{
    translator->head=translator->get_byte(translator->source);

    if(translator->end_of_data(translator->source))
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

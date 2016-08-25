#include "GetData.h"


char is_number(char c)
{
    if(c>='0' && c<='9')
        return 1;

    return 0;
}


char is_hex_number(char c)
{
    if((c>='0' && c<='9') || (c>='a' && c<='f') || (c>='A' && c<='F'))
        return 1;

    return 0;
}


char is_letter(char c)
{
    if((c>='a' && c<='z') || (c>='A' && c<='Z') || c=='_')
        return 1;

    return 0;
}


char is_space(char c)
{
    if(c==' ' || c=='\n' || c=='\r' || c=='\t')
        return 1;

    return 0;
}


void skip(Parser *parser)
{
    while(!parser->end_of_data && is_space(parser->head))
        read_byte(parser);
}


String* get_word(Parser *parser, String *token)
{
    if(is_number(parser->head))
    {
        printf("\nошибка чтения токена: первой буквой токена не может быть число");
        return token;
    }

    while((is_letter(parser->head) || is_number(parser->head)) && !parser->end_of_data)
    {
        str_push(token, parser->head);
        read_byte(parser);
    }

    return token;
}


String* get_new_token(Parser *parser)
{
    String *token=0;

    skip(parser);

    if(is_letter(parser->head))
    {
        token=str_init("");
        get_word(parser, token);
    }

    return token;
}


void get_token(Parser *parser, String *token)
{
    str_clear(token);
    skip(parser);
    get_word(parser, token);
}


char is_true_word(Parser *parser, char *word)
{
    unsigned int word_length=0,
                 symbol_pos_in_buffer;
    char         default_symbol=parser->head;

    for(; *word; word++)
    {
        if(parser->end_data(parser->source) || *word!=parser->head)
        {
            parser->head=default_symbol;
            parser->buffered_length+=word_length;

            if(word_length)
                parser->end_of_data=0;

            return 0;
        }

        read_byte(parser);

        symbol_pos_in_buffer=parser->buffered_length+word_length;

        if(symbol_pos_in_buffer == parser->buffer_length)
        {
            parser->buffer_length+=PARSER_BUFFER_SIZE;
            parser->buffer=realloc(parser->buffer, parser->buffer_length);
        }

        parser->buffer[symbol_pos_in_buffer]=parser->head;

        word_length++;
    }

    return 1;
}


void read_byte(Parser *parser)
{
    if(parser->buffered_symbol_pos<parser->buffered_length)
    {
        parser->head=parser->buffer[parser->buffered_symbol_pos];
        parser->buffered_symbol_pos++;

        if(parser->buffered_symbol_pos==parser->buffered_length)
        {
            parser->buffered_length=0;
            parser->buffered_symbol_pos=0;
        }
    }
    else
        parser->head=parser->get_byte(parser->source);

    if(parser->end_data(parser->source) && !parser->buffered_length)
        parser->end_of_data=1;
}

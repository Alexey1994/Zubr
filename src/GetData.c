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

void skip(Lexer *lexer)
{
    while(!lexer->end_of_data && is_space(lexer->head))
        lexer->read_byte(lexer);
}

String* get_word(Lexer *lexer, String *token)
{
    while(is_letter(lexer->head) && !lexer->end_of_data)
    {
        str_push(token, lexer->head);
        lexer->read_byte(lexer);
    }

    return token;
}

String* get_token_data(Lexer *lexer)
{
    String *token=0;

    skip(lexer);
    if(is_letter(lexer->head))
    {
        token=str_init("");
        get_word(lexer, token);
    }

    return token;
}

void get_token(Lexer *lexer, String *token)
{
    str_clear(token);
    skip(lexer);
    get_word(lexer, token);
}

char is_true_word(Lexer *lexer, char *word)
{
    unsigned int word_length=0,
                 symbol_pos_in_buffer;
    char         default_symbol=lexer->head;

    for(; *word; word++)
    {
        if(lexer->end_of_data || *word!=lexer->head)
        {
            lexer->head=default_symbol;
            lexer->buffered_length+=word_length;
            return 0;
        }

        lexer->read_byte(lexer);

        //if(!lexer->end_of_data)
        {
        symbol_pos_in_buffer=lexer->buffered_length+word_length;

        if(symbol_pos_in_buffer == lexer->buffer_length)
        {
            lexer->buffer_length+=LEXER_BUFFER_SIZE;
            lexer->buffer=realloc(lexer->buffer, lexer->buffer_length);
        }

        lexer->buffer[symbol_pos_in_buffer]=lexer->head;

        word_length++;
        }
    }

    return 1;
}

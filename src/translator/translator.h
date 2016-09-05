#ifndef TRANSLATOR_H_INCLUDED
#define TRANSLATOR_H_INCLUDED

#include "../prefix tree.h"
#include "../Array.h"
#include "../String.h"


typedef struct
{
    char   *in_source;
    char  (*get_byte)(char *source);
    char  (*end_of_data)(char *source);
    char    head;
    char    end_data;
    String *token;
    char   *out_source;
    void  (*out_byte)(char data, char *out_source);
}
TranslatorParser;


typedef struct
{
    PrefixTree *states;
}
Translator;


void init_translator_parser_states();

TranslatorParser* parse_translator(char  *in_source,
                                   char (*get_byte)(char *source),
                                   char (*end_of_data)(char *source),
                                   char  *out_source,
                                   void (*out_byte)(char data, char *source));

void convert_from_postfix_to_infix_notation(Array *expression);

#endif // TRANSLATOR_H_INCLUDED

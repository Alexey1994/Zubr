#ifndef TRANSLATOR_H_INCLUDED
#define TRANSLATOR_H_INCLUDED

#include "../parser/parser.h"
#include "../prefix tree.h"
#include "../Array.h"
#include "../String.h"


typedef struct
{
    char        *in_source;
    char       (*get_byte)(char *source);
    char       (*end_of_data)(char *source);
    char         head;
    char         end_data;
    String      *token;
    char        *out_source;
    void       (*out_byte)(char data, char *out_source);

    Array       *current_state;
    PrefixTree  *states;
}
TranslatorParser;


typedef struct
{
    char        *out_source;
    void       (*out_byte)(char data, char *out_source);
    PrefixTree  *commands;
    Array       *stack_blocks;
    Array       *current_body;
    int          current_i;

    Array       *current_expression;
}
Translator;


void init_translator_parser_states();
void init_translator_table();

PrefixTree* parse_translator(char  *in_source,
                             char (*get_byte)(char *source),
                             char (*end_of_data)(char *source),
                             char  *out_source,
                             void (*out_byte)(char data, char *source));

void translate(Function *main, PrefixTree *translator_commands, char  *out_source, void (*out_byte)(char data, char *source));


#endif // TRANSLATOR_H_INCLUDED

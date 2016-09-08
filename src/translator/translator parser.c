#include "translator parser.h"

#include "translator.h"
#include "get data.h"
#include "types.h"

#include "parser commands/push.h"
#include "parser commands/out.h"
#include "parser commands/state.h"
#include "parser commands/expression.h"


static PrefixTree* translator_states;
static PrefixTree* translator_commands;


void init_translator_parser_states()
{
    PrefixTree *state;

    translator_states=create_prefix_tree();

    state=create_prefix_tree();
    add_data_in_prefix_tree(state, "condition", 1);

    add_data_in_prefix_tree(translator_states, "if", state);


    state=create_prefix_tree();
    add_data_in_prefix_tree(state, "condition", 1);

    add_data_in_prefix_tree(translator_states, "while", state);


    translator_commands=create_prefix_tree();

    add_data_in_prefix_tree(translator_commands, "out"  , translator_parser_command_out);
    add_data_in_prefix_tree(translator_commands, "push" , translator_parser_command_push);
    add_data_in_prefix_tree(translator_commands, "state", translator_parser_command_state);
    add_data_in_prefix_tree(translator_commands, "expression", translator_parser_command_expression);
}


void parse_translator_body(TranslatorParser *parser, PrefixTree *state)
{
    void (*command)(TranslatorParser *parser);
    char  *command_name;

    skip_translator_parser(parser);

    if(state)
    if(find_data_in_prefix_tree(state, "condition"))
        printf("<condition>");

    while(parser->head!='}' && !parser->end_data)
    {
        command_name=get_token_translator_parser(parser);

        command=0;

        if(state)
            command=find_data_in_prefix_tree(state, command_name);

        if(command)
            command(parser);
        else
        {
            command=find_data_in_prefix_tree(translator_commands, command_name);

            if(!command)
            {
                printf("комманда %s не найдена\n", command_name);
                return;
            }

            command(parser);
        }

        skip_translator_parser(parser);
    }
}


char parse_translator_state(TranslatorParser *parser)
{
    char *state_name;

    state_name=get_new_token_translator_parser(parser);

    if(!state_name)
    {
        printf("отсутствует токен\n");
        return 0;
    }

    printf("state: %s\n", state_name);

    skip_translator_parser(parser);

    if(parser->head!='{')
    {
        printf("отсутствует {\n");
        return 0;
    }

    parser->current_state=array_init(4);

    get_byte_translator_parser(parser);
    parse_translator_body(parser, find_data_in_prefix_tree(translator_states, state_name));
    get_byte_translator_parser(parser);

    add_data_in_prefix_tree(parser->states, state_name, parser->current_state);
    parser->current_state=0;

    return 1;
}


PrefixTree* parse_translator(char  *in_source,
                                   char (*get_byte)(char *source),
                                   char (*end_of_data)(char *source),
                                   char  *out_source,
                                   void (*out_byte)(char data, char *source))
{
    TranslatorParser *parser     = new(TranslatorParser);
    PrefixTree       *translator;

    parser->end_of_data   = end_of_data;
    parser->get_byte      = get_byte;
    parser->in_source     = in_source;
    parser->end_data      = 0;
    parser->out_source    = out_source;
    parser->out_byte      = out_byte;
    parser->token         = str_init("");
    parser->current_state = 0;
    parser->states        = create_prefix_tree();

    get_byte_translator_parser(parser);

    while(!parser->end_data)
    {
        if(!parse_translator_state(parser))
            return 0;
    }

    translator=parser->states;

    return translator;
}

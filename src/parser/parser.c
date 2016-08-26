//1.Ok
#include <locale.h>
#include "parser.h"
#include "expression.h"
#include "../SystemLibrary.h"
#include "../prefix tree.h"

#include "tokens/print.h"
#include "tokens/break.h"
#include "tokens/continue.h"
#include "tokens/if.h"
#include "tokens/while.h"
#include "tokens/do.h"
#include "tokens/else.h"
#include "tokens/end.h"
#include "tokens/loop.h"
#include "tokens/push.h"
#include "tokens/include.h"


static String     *main_name;
static PrefixTree *parser_tokens;


void init_parser()
{
    setlocale(LC_ALL, "rus");
    init_expression();
    main_name=str_init("");

    parser_tokens=create_prefix_tree();

    add_data_in_prefix_tree(parser_tokens, "print"   , parser_print   );
    add_data_in_prefix_tree(parser_tokens, "if"      , parser_if      );
    add_data_in_prefix_tree(parser_tokens, "else"    , parser_else    );
    add_data_in_prefix_tree(parser_tokens, "end"     , parser_end     );
    add_data_in_prefix_tree(parser_tokens, "loop"    , parser_loop    );
    add_data_in_prefix_tree(parser_tokens, "while"   , parser_while   );
    add_data_in_prefix_tree(parser_tokens, "do"      , parser_do      );
    add_data_in_prefix_tree(parser_tokens, "break"   , parser_break   );
    add_data_in_prefix_tree(parser_tokens, "continue", parser_continue);
    add_data_in_prefix_tree(parser_tokens, "push"    , parser_push    );
    add_data_in_prefix_tree(parser_tokens, "include" , parser_include );
}


char get_function_body(Parser *parser)
{
    char (*func)(Parser *parser_data);

    parser->is_end_function=0;

    while(!parser->is_end_function)
    {
        if(parser->end_of_data)
        {
            if(!parser->sources->length)
                break;

            parser->source=array_pop(parser->scopes);
        }

        get_token(parser, parser->token);
        func=find_data_in_prefix_tree(parser_tokens, parser->token->begin);

        if(func)
        {
            if(!func(parser))
                return 0;
        }
        else
        {
            skip(parser);

            if(parser->head==':')
            {
                if(!get_function(parser, str_copy(parser->token)))
                    return 0;
            }
            else if(parser->head=='#')
            {
                do
                    read_byte(parser);
                while(parser->head!='\n' && !parser->end_of_data);

                skip(parser);
            }
            else if(!get_call_or_assignment(parser))
            {
                if(!parser_load_class(parser))
                {
                    if(!parser->token->length)
                    {
                        printf("\nнеопределённый символ '%c'", parser->head);
                        return 0;
                    }

                    if(!parser_get_variable(parser, str_copy(parser->token), UNDEFINED))
                        return 0;
                }
            }
        }
    }

    parser->is_end_function=0;

    return 1;
}


static int system_library_str_comparision(String *name, SystemLibrary *library)
{
    return str_comparision(name, library->name);
}


static int system_library_comparision(SystemLibrary *library1, SystemLibrary *library2)
{
    return str_comparision(library1->name, library2->name);
}

static void free_system_library_node(SystemLibrary *library)
{

}


Function* run_parser(char *source, char (*get_byte)(Parser *parser), char (*end_of_data)(Parser *parser))
{
    Parser   *parser_alloc            = new(Parser);
    Function *main                    = new_function(main_name, 0);

    main->return_var                  = 0;

    parser_alloc->end_of_data         = 0;
    parser_alloc->source              = source;
    parser_alloc->get_byte            = get_byte;
    parser_alloc->end_data            = end_of_data;
    parser_alloc->token               = str_init("");
    parser_alloc->blocks_pos          = array_init(2);
    parser_alloc->scopes              = array_init(1);
    parser_alloc->id                  = init_id(128, 255);
    parser_alloc->expr_token          = str_init("");

    parser_alloc->cur_function        = main;
    parser_alloc->cur_body            = main->body;
    parser_alloc->cur_block           = new_data(main, FUNCTION);
    parser_alloc->cur_scope           = parser_alloc->cur_function->variables;

    parser_alloc->buffer              = malloc(PARSER_BUFFER_SIZE);
    parser_alloc->buffer_length       = PARSER_BUFFER_SIZE;
    parser_alloc->buffered_length     = 0;
    parser_alloc->buffered_symbol_pos = 0;

    parser_alloc->sources             = array_init(4);
    parser_alloc->system_libraries    = tree_init(system_library_str_comparision, system_library_comparision, free_system_library_node);

    parser_alloc->stack_infered_types = array_init(10);

    read_byte(parser_alloc);

    if(!get_function_body(parser_alloc))
        return 0;

    if(!array_empty(parser_alloc->blocks_pos))
    {
        printf("отсутствует end\n");
        return 0;
    }

    return main;
}

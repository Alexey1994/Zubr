#include "lexer.h"
#include "../String.h"
#include "../extends.h"
#include "lexer_operations.h"

static String *main_name;

typedef struct
{
    String *token;
    char  (*func)(Lexer *lexer_data);
}
LexerTableData;

static Tree *lexer_table;


static LexerTableData* new_lexer_table_data(String *token, char (*func)(Lexer *lexer_data))
{
    LexerTableData *data=new(LexerTableData);

    data->token=token;
    data->func=func;

    return data;
}


static int lexer_data_cmp(LexerTableData *data1, LexerTableData *data2)
{
    return str_comparision(data1->token, data2->token);
}


static int lexer_token_cmp(String *token, LexerTableData *data)
{
    return str_comparision(token, data->token);
}


void lexer_init()
{
    main_name=str_init("");

    lexer_table=tree_init();

    tree_add(lexer_table, new_lexer_table_data(str_init("print"   ), lexer_print   ), lexer_data_cmp);
    tree_add(lexer_table, new_lexer_table_data(str_init("if"      ), lexer_if      ), lexer_data_cmp);
    tree_add(lexer_table, new_lexer_table_data(str_init("else"    ), lexer_else    ), lexer_data_cmp);
    tree_add(lexer_table, new_lexer_table_data(str_init("var"     ), lexer_var     ), lexer_data_cmp);
    tree_add(lexer_table, new_lexer_table_data(str_init("end"     ), lexer_end     ), lexer_data_cmp);
    tree_add(lexer_table, new_lexer_table_data(str_init("loop"    ), lexer_loop    ), lexer_data_cmp);
    tree_add(lexer_table, new_lexer_table_data(str_init("while"   ), lexer_while   ), lexer_data_cmp);
    tree_add(lexer_table, new_lexer_table_data(str_init("do"      ), lexer_do      ), lexer_data_cmp);
    tree_add(lexer_table, new_lexer_table_data(str_init("break"   ), lexer_break   ), lexer_data_cmp);
    tree_add(lexer_table, new_lexer_table_data(str_init("continue"), lexer_continue), lexer_data_cmp);
    tree_add(lexer_table, new_lexer_table_data(str_init("function"), lexer_function), lexer_data_cmp);
    tree_add(lexer_table, new_lexer_table_data(str_init("return"  ), lexer_return  ), lexer_data_cmp);
    tree_add(lexer_table, new_lexer_table_data(str_init("push"    ), lexer_push    ), lexer_data_cmp);
    tree_add(lexer_table, new_lexer_table_data(str_init("include" ), lexer_include ), lexer_data_cmp);
    tree_add(lexer_table, new_lexer_table_data(str_init("N"       ), lexer_N       ), lexer_data_cmp);
}


char get_function_body(Lexer *lexer)
{
    LexerTableData *lexer_data;

    lexer->is_end_function=0;//?

    while(!lexer->is_end_function)
    {
        if(lexer->end_of_data)
        {
            if(!lexer->sources->length)
                break;

            lexer->source=array_pop(lexer->scopes);
        }

        get_token(lexer, lexer->token);

        lexer_data=tree_find(lexer_table, lexer->token, lexer_token_cmp);
        if(lexer_data)
        {
            if(!lexer_data->func(lexer))
                return 0;
        }
        else
        {
            if(lexer->head=='#')
            {
                do
                    read_byte(lexer);
                while(lexer->head!='\n' && !lexer->end_of_data);
            }
            else if(!get_call_or_assignment(lexer))
            {
                if(!lexer_load_class(lexer))
                {
                    printf("\n������������� ������ '%c'", lexer->head);
                    return 0;
                }
            }
        }
    }

    lexer->is_end_function=0;

    return 1;
}


Function* run_lexer(char *source, char (*get_byte)(Lexer *lexer), char (*end_of_data)(Lexer *lexer))
{
    Lexer    *lexer_alloc=new(Lexer);
    Function *main=new_function(main_name, 0);

    lexer_alloc->end_of_data=0;
    lexer_alloc->source=source;
    lexer_alloc->get_byte=get_byte;
    lexer_alloc->end_data=end_of_data;

    lexer_alloc->token=str_init("");

    lexer_alloc->blocks_pos=stack_init();
    lexer_alloc->scopes=stack_init();

    lexer_alloc->id=id_init(128, 255);
    lexer_alloc->expr_token=str_init("");

    lexer_alloc->cur_function=main;
    lexer_alloc->cur_body=main->body;
    lexer_alloc->cur_block=new_data(main, FUNCTION);
    lexer_alloc->cur_scope=lexer_alloc->cur_function->variables;

    lexer_alloc->buffer=malloc(LEXER_BUFFER_SIZE);
    lexer_alloc->buffer_length=LEXER_BUFFER_SIZE;
    lexer_alloc->buffered_length=0;
    lexer_alloc->buffered_symbol_pos=0;

    lexer_alloc->sources=array_init();

    read_byte(lexer_alloc);

    if(!get_function_body(lexer_alloc))
        return 0;

    if(lexer_alloc->blocks_pos->begin)
    {
        printf("����������� end\n");
    }

    return main;
}

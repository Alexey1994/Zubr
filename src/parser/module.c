#include "module.h"
#include "../SystemLibrary.h"


static char get_next_object(Parser *parser, Function *class)
{
    String     *var_name=get_new_token(parser);
    Assignment *assignment;
    Variable   *variable;

    //interpretator(lib_main);

    variable=new_static_variable(parser, var_name, MODULE, class);

    if(!variable)
        return 0;

    return 1;
}

static char get_objects(Parser *parser, Function *class)
{
    do
    {
        read_byte(parser);

        if(!get_next_object(parser, class))
            return 0;

        skip(parser);
    }
    while(parser->head==',');

    return 1;
}


static int system_library_comparision(SystemLibrary *library1, SystemLibrary *library2)
{
    return str_comparision(library1->name, library2->name);
}


SystemLibrary* find_system_library(Parser *parser, String *name)
{
    return tree_find(parser->system_libraries, name);
}


void add_system_library(Parser *parser, SystemLibrary *library)
{
    tree_add(parser->system_libraries, library);
}


Array* get_call_function_args(Parser *parser)
{
    Array *args=array_init(8);
    Array *arg_expr;

    skip(parser);
    if(parser->head!='(')
    {
        printf("expected (\n");
        array_free(args);
        return 0;
    }

    read_byte(parser);

    while(!parser->end_of_data)
    {
        skip(parser);

        if(parser->head==')')
            break;

        arg_expr=parser_get_expression(parser);
        if(!arg_expr)
        {
            array_free(args);
            return 0;
        }
        array_push(args, arg_expr);
        skip(parser);

        if(parser->head!=',')
            break;

        read_byte(parser);
    }

    if(parser->head!=')')
    {
        array_free(args);
        printf("\nотсутствует )");
        return 0;
    }
    read_byte(parser);

    return args;
}


SystemCall* parser_get_system_call(Parser *parser, String *library_name)
{
    SystemLibrary *library;
    String        *system_function_name;
    int          (*system_function)();
    Array         *system_function_args;
    SystemCall    *system_function_call;

    library=find_system_library(parser, library_name);

    if(!library)
    {
        library=open_system_library(str_copy(library_name));

        if(!library)
            return 0;

        add_system_library(parser, library);
    }

    skip(parser);

    if(parser->head!='.')
    {
        printf("\nотсутствует . в системном вызове");
        return 0;
    }

    read_byte(parser);

    system_function_name=get_new_token(parser);
    system_function=get_system_function(library, system_function_name);

    if(!system_function)
    {
        printf("\nфункция ");
        str_print(parser->token);
        printf(" не найдена в библиотеке ");
        str_print(library->name);
        return 0;
    }

    system_function_args=get_call_function_args(parser);

    if(!system_function_args)
        return 0;

    system_function_call=new(SystemCall);
    system_function_call->function = system_function;
    system_function_call->args     = system_function_args;
}


char parser_load_class(Parser *parser)
{
    FILE          *f;
    Function      *class;
    SystemCall    *system_function_call;

    f=fopen(parser->token->begin, "rb");

    if(f)
    {
        class=run_parser(f, parser->get_byte, parser->end_data);
        //interpretator(class);

        fclose(f);
        return get_objects(parser, class);
    }

    system_function_call=parser_get_system_call(parser, parser->token);

    if(!system_function_call)
        return 0;

    array_push(parser->cur_body, new_data(system_function_call, SYSTEM_CALL));

    return 1;
}

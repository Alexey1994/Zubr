#include "operand.h"
#include "../Map.h"


static char get_args(Parser *parser)
{
    String   *arg_name;
    Variable *arg;
    char      is_braces=0;

    if(parser->head=='(')
    {
        read_byte(parser);
        is_braces=1;
    }

    skip(parser);

    while(parser->head!=')' && !parser->end_of_data)
    {
        arg_name=get_new_token(parser);

        if(!arg_name)
            return 0;

        arg=new_variable(parser, arg_name, ARGUMENT);
        if(!arg)
            return 0;

        array_push(parser->cur_function->args, arg);

        skip(parser);

        if(parser->head!=',')
            break;

        read_byte(parser);
        skip(parser);
    }

    if(parser->head==')')
    {
        if(!is_braces)
        {
            printf("отсутствует открывающая скобка\n");
            return 0;
        }

        read_byte(parser);
    }
    else if(is_braces)
    {
        printf("отсутствует закрывающая скобка\n");
        return 0;
    }

    return 1;
}


Variable* get_function(Parser *parser, String *function_name)
{
    Function *f;
    Variable *return_variable_name;
    Variable *function_variable;

    read_byte(parser);

    f=new_function(function_name);
    function_variable=new_static_variable(parser, function_name, FUNCTION, f);

    if(!function_variable)
    {
        free(f);
        return 0;
    }

    parser->cur_function=f;

    array_push(parser->blocks_pos, parser->cur_block);
    parser->cur_block=new_data(f, FUNCTION);

    array_push(parser->scopes, parser->cur_scope);
    parser->cur_scope=f->variables;

    parser->cur_body=f->body;
    parser->cur_function=f;

    skip(parser);

    if(!get_args(parser))
    {
        free(f);
        return 0;
    }

    skip(parser);

    if(is_true_word(parser, "->"))
    {
        skip(parser);

        return_variable_name=get_new_token(parser);
        f->return_var=new_variable(parser, return_variable_name, ARGUMENT);

        if(!f->return_var)
            return 0;
    }
    else
    {
        f->return_var=0;
    }

    get_function_body(parser);

    return function_variable;
}


static Variable* get_map(Parser *parser)
{
    String     *key;
    Variable   *map_variable;
    Map        *map=map_init();
    Assignment *assignment;

    read_byte(parser);
    map_variable=new_static_variable(parser, str_init(generate_id(parser->id)), MAP, map);

    while(parser->head!='}')
    {
        key=get_new_token(parser);

        if(!key)
        {
            printf("неправильный ключ\n");
            return 0;
        }

        skip(parser);

        if(parser->head!=':')
        {
            printf("отсутствует :\n");
            return 0;
        }

        read_byte(parser);

        assignment                   = new(Assignment);
        assignment->left_operand     = array_init(4);
        assignment->right_expression = parser_get_expression(parser);

        if(!assignment->right_expression)
        {
            return 0;
        }

        array_push(assignment->left_operand, new_data(map_variable, 'v'));
        array_push(assignment->left_operand, new_data(key, '.'));

        array_push(parser->cur_body, new_data(assignment, ASSIGNMENT));

        skip(parser);

        if(parser->end_of_data)
        {
            printf("отсутствует }");
            return 0;
        }
    }

    read_byte(parser);

    return map_variable;
}


static Variable* get_array(Parser *parser)
{
    Variable *array_var;
    Array    *array;
    Variable *array_index;
    Array    *array_element;
    Push     *push_data;

    read_byte(parser);
    skip(parser);

    array=array_init(8);
    array_var=new_static_variable(parser, str_init(generate_id(parser->id)), ARRAY, array);

    while(parser->head!=']')
    {
        push_data=new(Push);

        array_element=parser_get_expression(parser);

        if(!array_element)
            goto error;

        push_data->array=array_init(8);
        array_push(push_data->array, new_data(array_var, 'v'));
        push_data->expression=array_element;

        array_push(parser->cur_body, new_data(push_data, PUSH));

        if(parser->head==',')
            read_byte(parser);

        skip(parser);

        if(parser->end_of_data)
        {
            printf("отсутствует ]");
            return 0;
        }
    }

    read_byte(parser);
    skip(parser);

    return array_var;

error:
    //array_free(array);
    return 0;
}


static Variable* get_number(Parser *parser)
{
    Variable *number;
    char      is_real=0;
    float     real_value;

    str_clear(parser->token);

    if(is_true_word(parser, "0x") || is_true_word(parser, "0X"))
    {
        while(parser->head=='0')
            read_byte(parser);

        while(is_hex_number(parser->head) && !parser->end_of_data)
        {
            str_push(parser->token, parser->head);
            read_byte(parser);
        }

        number=new_static_variable(parser, str_init(generate_id(parser->id)), CONST_INTEGER, str_hex_to_int(parser->token));

        return number;
    }

    do
    {
        str_push(parser->token, parser->head);
        read_byte(parser);
        skip(parser);

        if(parser->head=='.')
        {
            if(is_real)
            {
                printf("\nнеправильная запись вещественного числа-две точки");
                return 0;
            }

            is_real=1;

            str_push(parser->token, ',');
            read_byte(parser);
            skip(parser);
        }
    }
    while(!parser->end_of_data && (is_number(parser->head) || parser->head=='.'));

    if(is_real)
    {
        real_value=str_to_float(parser->token);

        return new_static_variable(parser, str_init(generate_id(parser->id)), CONST_REAL, *(int*)&real_value);
    }
    else
        return new_static_variable(parser, str_init(generate_id(parser->id)), CONST_INTEGER, str_to_int(parser->token));
}


static Variable* get_string(Parser *parser)
{
    char    format_quotes = parser->head;
    String *const_string  = str_init("");

    read_byte(parser);

    while(parser->head!=format_quotes)
    {
        if(parser->end_of_data)
        {
            str_free(const_string);
            printf("\nотсутствует %c", format_quotes);
            return 0;
        }

        if(parser->head=='\\')
        {
            read_byte(parser);

            switch(parser->head)
            {
                case 'n': str_push(const_string, '\n'); break;
                case 't': str_push(const_string, '\t'); break;
                case 'b': str_push(const_string, '\b'); break;
                case 'r': str_push(const_string, '\r'); break;
                case '\\': str_push(const_string, '\\'); break;
                case '\'': str_push(const_string, '\''); break;
                case '"': str_push(const_string, '"'); break;
            }
        }
        else
            str_push(const_string, parser->head);

        read_byte(parser);
    }

    read_byte(parser);

    return new_static_variable(parser, str_init(generate_id(parser->id)), STRING, const_string);
}


char get_operand(Parser *parser, Array *expression)
{
    Variable *var;
    Array    *compound_operand;

    skip(parser);

    if(parser->head==':' || is_true_word(parser, "function"))
        var=get_function(parser, str_init(generate_id(parser->id)));

    else if(is_letter(parser->head))
    {
        get_token(parser, parser->token);
        compound_operand=get_compound_operand(parser);

        if(!compound_operand)
            return 0;

        array_push(expression, new_data(compound_operand, COMPOUND_OPERAND));

        return 1;
    }

    else if(is_number(parser->head))
        var=get_number(parser);

    else if(parser->head=='[')
        var=get_array(parser);

    else if(parser->head=='{')
        var=get_map(parser);

    else if(parser->head=='\'' || parser->head=='"')
        var=get_string(parser);

    else
    {
        printf("неопределённый операнд, ожидалось [, {, ', \" либо имя переменной\n");
        return 0;
    }

    if(!var)
        return 0;

    array_push(expression, new_data(var, OPERAND));

    return 1;
}


Array* get_compound_operand(Parser *parser)
{
    Variable   *variable;
    Array      *operand=array_init(4),
               *args_expr;
    Call       *call;
    char        is_break=0;
    String     *key;
    String     *function_name;
    SystemCall *system_call;

    variable=find_variable(parser->token, parser);

    if(!variable)
    {
        system_call=parser_get_system_call(parser, parser->token);

        if(!system_call)
            return 0;

        array_push(operand, new_data(system_call, 's'));

        return operand;
    }
    else
        array_push(operand, new_data(variable, 'v'));

    loop
    {
        skip(parser);

        switch(parser->head)
        {
        case '.':
            read_byte(parser);

            key=get_new_token(parser);

            array_push(operand, new_data(key, '.'));

            str_clear(parser->token);
            //get_token(parser->f, parser->head, parser->token);
            continue;

        case '(':
            args_expr=get_call_function_args(parser);

            if(!args_expr)
                return 0;

            array_push(operand, new_data(args_expr, '('));
            break;

        case '[':
            read_byte(parser);
            variable=parser_get_expression(parser);
            if(!variable)
                return 0;

            if(parser->head!=']')
            {
                printf("expected ]\n");
                return 0;
            }
            read_byte(parser);

            array_push(operand, new_data(variable, '['));
            break;
        }

        if(parser->head!='.' && parser->head!='(' && parser->head!='[')
            return operand;

        str_clear(parser->token);
        get_token(parser, parser->token);
    }
}

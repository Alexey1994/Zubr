#include "lexer_operations.h"
#include "../extends.h"
#include "../String.h"
#include "../types.h"
#include "../List.h"
#include "../GetData.h"


static int variable_cmp(Variable *var1, Variable *var2)
{
    return str_comparision(var1->name, var2->name);
}


static int variable_str_var_cmp(String *s, Variable *var)
{
    return str_comparision(s, var->name);
}


static Variable* find_local_variable(Tree *variables, String *name)
{
    return tree_find(variables, name, variable_str_var_cmp);
}


static Variable* add_variable(Lexer *lexer, String *name)
{
    Variable *var;

    if(find_local_variable(lexer->cur_function->variables, name))
    {
        printf("\nпеременная ");
        str_print(name);
        printf(" уже существует");
        return 0;
    }

    var=new(Variable);
    var->name=name;

    tree_add(lexer->cur_scope, var, variable_cmp);

    return var;
}


Variable* new_variable(Lexer *lexer, String *name, char type)
{
    Variable *var=add_variable(lexer, name);

    if(!var)
        return 0;

    var->type=type;
    var->is_closed=0;

    lexer->cur_function->count_vars++;
    var->shift=lexer->cur_function->count_vars;

    return var;
}


Variable* new_static_variable(Lexer *lexer, String *name, char type, int *data)
{
    Variable *var=add_variable(lexer, name);

    if(!var)
        return 0;

    var->type=type;
    var->shift=data;

    return var;
}


static Variable* get_global_variable(String *name, Lexer *lexer)
{
    Variable  *finded_var;
    NodeStack *i;

    for(i=lexer->scopes->begin; i; i=i->previouse)
    {
        finded_var=find_local_variable(i->data, name);

        if(finded_var)
        {
            finded_var->is_closed=1;
            return finded_var;
        }
    }

    //printf("\nне определённая переменная ");
    //str_print(name);

    return 0;
}


Variable* find_variable(String *name, Lexer *lexer)
{
    Variable  *finded_var;

    finded_var=find_local_variable(lexer->cur_function->variables, name);

    if(!finded_var)
        finded_var=get_global_variable(name, lexer);

    return finded_var;
}


Function* new_function(String *name)
{
    Function *f=new(Function);

    f->name       = name;
    f->args       = array_init();
    f->body       = list_init();
    f->functions  = tree_init();
    f->return_var = 0;
    f->variables  = tree_init();
    f->count_vars = 0;

    return f;
}


static void add_block(Lexer *lexer, Data *new_block, char block_type, List *block_body)
{
    Data *block_data=new_data(new_block, block_type);

    push(lexer->blocks_pos, lexer->cur_block);
    list_push(lexer->cur_body, block_data);
    lexer->cur_body=block_body;
    lexer->cur_block=block_data;
}


//----PRINT--------------------------------------------------------------------------

static char get_print(Lexer *lexer)
{
    List     *expression;
    Print    *print_data;

    expression=lexer_get_expression(lexer);
    if(!expression)
        return 0;

    print_data=new(Print);
    print_data->expression=expression;
    list_push(lexer->cur_body, new_data(print_data, PRINT));

    return 1;
}

char lexer_print(Lexer *lexer)
{
    do
    {
        read_byte(lexer);

        if(!get_print(lexer))
            return 0;

        skip(lexer);
    }
    while(lexer->head==',');

    return 1;
}

//----IF-----------------------------------------------------------------------------

char lexer_if(Lexer *lexer)
{
    List *condition;
    If   *if_data;

    condition=lexer_get_condition(lexer);
    if(!condition)
        return 0;

    if_data=new(If);
    if_data->cond=condition;
    if_data->body=list_init();
    if_data->else_body=list_init();

    add_block(lexer, if_data, IF, if_data->body);

    return 1;
}

//----ELSE---------------------------------------------------------------------------

char lexer_else(Lexer *lexer)
{
    If *if_data,
       *else_if_data,
       *cur_if;

    if(lexer->cur_block->type==IF)
    {
        if_data=lexer->cur_block->data;
        lexer->cur_body=if_data->else_body;

        skip(lexer);
        if(is_true_word(lexer, "if"))
        {
            else_if_data=new(If);
            else_if_data->body=list_init();
            else_if_data->else_body=list_init();
            else_if_data->cond=lexer_get_expression(lexer);

            lexer->cur_block=new_data(else_if_data, IF);

            list_push(if_data->else_body, lexer->cur_block);
            lexer->cur_body=else_if_data->body;
        }
        else
            lexer->cur_block=new_data(if_data->body, ELSE);
    }
    else
    {
        printf("else находится не в if\n");
        return 0;
    }

    return 1;
}

//----VAR----------------------------------------------------------------------------

static char get_next_var(Lexer *lexer, char type)
{
    String     *var_name=get_token_data(lexer);
    Assignment *assignment;
    Variable   *variable=new_variable(lexer, var_name, type);
    List       *expression;

    if(!variable)
        return 0;

    skip(lexer);
    if(lexer->head=='=')
    {
        read_byte(lexer);
        str_clear(lexer->token);

        expression=lexer_get_expression(lexer);
        if(!expression)
            return 0;

        assignment=new(Assignment);
        assignment->left_operand=array_init();
        assignment->right_expression=expression;

        array_push(assignment->left_operand, new_data(variable, 'v'));
        list_push(lexer->cur_body, new_data(assignment, ASSIGNMENT));
    }

    return 1;
}

static char get_var(Lexer *lexer, char type)
{
    do
    {
        read_byte(lexer);

        if(!get_next_var(lexer, type))
            return 0;

        skip(lexer);
    }
    while(lexer->head==',');

    return 1;
}

char lexer_var(Lexer *lexer)
{
    if(!get_var(lexer, UNDEFINED))
        return 0;

    return 1;
}

char lexer_N(Lexer *lexer)
{
    if(!get_var(lexer, INTEGER))
        return 0;

    return 1;
}

//----END----------------------------------------------------------------------------

static void update_cur_body(Lexer *lexer)
{
    While    *while_data;
    If       *if_data;
    Function *function_data;
    Data     *cur_block=lexer->cur_block;

    switch(cur_block->type)
    {
    case IF:
        if_data=cur_block->data;
        lexer->cur_body=if_data->body;
        break;

    case WHILE:
        while_data=cur_block->data;
        lexer->cur_body=while_data->body;
        break;

    case ELSE:
    case LOOP:
    case FUNCTION:
        function_data=cur_block->data;
        lexer->cur_body=function_data->body;
        lexer->cur_function=function_data;
        break;
    }
}

char lexer_end(Lexer *lexer)
{
    if(!lexer->blocks_pos->begin)
    {
        printf("end не принадлежит ни одному блоку\n");
        return 0;
    }

    switch(lexer->cur_block->type)
    {
        case FUNCTION:
            lexer->cur_scope=pop(lexer->scopes);
            lexer->is_end_function=1;
            break;

        case ELSE:
            free(lexer->cur_block);
            break;

        case DO:
            printf("do без while\n");
            return 0;
            break;
    }

    lexer->cur_block=pop(lexer->blocks_pos);
    update_cur_body(lexer);

    return 1;
}

//----LOOP---------------------------------------------------------------------------

char lexer_loop(Lexer *lexer)
{
    List *loop_body=list_init();
    add_block(lexer, loop_body, LOOP, loop_body);
    return 1;
}

//----WHILE--------------------------------------------------------------------------

char lexer_while(Lexer *lexer)
{
    While *while_data;
    Do    *do_data;
    List  *cond;

    cond=lexer_get_condition(lexer);
    if(!cond)
        return 0;

    if(lexer->cur_block->type==DO)
    {
        do_data=lexer->cur_block->data;
        do_data->cond=cond;

        lexer->cur_block=pop(lexer->blocks_pos);
        update_cur_body(lexer);
    }
    else
    {
        while_data=new(While);
        while_data->cond=cond;
        while_data->body=list_init();

        add_block(lexer, while_data, WHILE, while_data->body);
    }

    return 1;
}

//----DO-----------------------------------------------------------------------------

char lexer_do(Lexer *lexer)
{
    Do    *do_data;

    do_data=new(Do);
    do_data->body=list_init();

    add_block(lexer, do_data, DO, do_data->body);

    return 1;
}

//----FUNCTION-----------------------------------------------------------------------

static char get_args(Lexer *lexer)
{
    String   *arg_name;
    Variable *arg;
    char      is_braces=0;

    if(lexer->head=='(')
    {
        read_byte(lexer);
        is_braces=1;
    }

    skip(lexer);

    while(lexer->head!=')' && !lexer->end_of_data)
    {
        arg_name=get_token_data(lexer);
        if(!arg_name)
            return 0;

        arg=new_variable(lexer, arg_name, ARGUMENT);
        if(!arg)
            return 0;

        array_push(lexer->cur_function->args, arg);

        skip(lexer);

        if(lexer->head!=',')
            break;

        read_byte(lexer);
        skip(lexer);
    }

    if(lexer->head==')')
    {
        if(!is_braces)
        {
            printf("отсутствует открывающая скобка\n");
            return 0;
        }

        read_byte(lexer);
    }
    else if(is_braces)
    {
        printf("отсутствует закрывающая скобка\n");
        return 0;
    }

    return 1;
}


Function* get_function(Lexer *lexer, String *function_name)
{
    Function *f;
    Variable *return_variable_name;

    if(find_local_variable(lexer->cur_function->variables, function_name))
    {
        printf("имя ");
        str_print(function_name);
        printf(" уже используется\n");

        return 0;
    }

    f=new_function(function_name);
    lexer->cur_function=f;
    new_static_variable(lexer, function_name, FUNCTION, f);

    push(lexer->blocks_pos, lexer->cur_block);
    lexer->cur_block=new_data(f, FUNCTION);

    push(lexer->scopes, lexer->cur_scope);
    lexer->cur_scope=f->variables;

    lexer->cur_body=f->body;
    lexer->cur_function=f;

    skip(lexer);

    if(!get_args(lexer))
    {
        free(f);
        return 0;
    }

    skip(lexer);
    //if(is_true_word(lexer, "->") || lexer->head=='=')
    if(lexer->head=='=')
    {
        read_byte(lexer);
        skip(lexer);

        return_variable_name=get_token_data(lexer);
        f->return_var=new_variable(lexer, return_variable_name, ARGUMENT);

        if(!f->return_var)
            return 0;
    }
    else
    {
        f->return_var=0;
    }

    get_function_body(lexer);

    return f;
}

char lexer_function(Lexer *lexer)
{
    String   *function_name;
    Function *f;

    function_name=get_token_data(lexer);
    if(!function_name)
        return 0;

    if(!get_function(lexer, function_name))
        return 0;

    return 1;
}

//----DEFINED  OPERAND---------------------------------------------------------------

static Array* get_call_function_args(Lexer *lexer)
{
    Array *args=array_init();
    List  *arg_expr;

    skip(lexer);
    if(lexer->head!='(')
    {
        printf("expected (\n");
        array_free(args);
        return 0;
    }

    read_byte(lexer);

    while(!lexer->end_of_data)
    {
        skip(lexer);

        if(lexer->head==')')
            break;

        arg_expr=lexer_get_expression(lexer);
        if(!arg_expr)
        {
            array_free(args);
            return 0;
        }
        array_push(args, arg_expr);
        skip(lexer);

        if(lexer->head!=',')
            break;

        read_byte(lexer);
    }

    if(lexer->head!=')')
    {
        array_free(args);
        printf("\nотсутствует )");
        return 0;
    }
    read_byte(lexer);

    return args;
}

static Call* get_function_call(Lexer *lexer, String *name)
{
    Variable *function_variable;
    Function *function;
    Array    *args;
    Call     *call;

    function_variable=find_variable(name, lexer);
    if(!function_variable)
        return 0;

    args=get_call_function_args(lexer);
    if(!args)
        return 0;

    call=new(Call);
    call->args=args;
    call->function=function_variable;

    return call;
}

Array* get_defined_operand(Lexer *lexer)
{
    Variable *variable;
    Array    *operand=array_init(),
             *args_expr;
    Call     *call;
    char      is_break=0;
    String   *key;
    String   *function_name;

    variable=find_variable(lexer->token, lexer);
    if(!variable)
    {
        if(lexer->head==':')
        {
            read_byte(lexer);
            skip(lexer);

            function_name=str_copy(lexer->token);

            if(!get_function(lexer, function_name))
                return 0;
        }
        else
            return 0;
    }

    array_push(operand, new_data(variable, 'v'));

    loop
    {
        skip(lexer);

        switch(lexer->head)
        {
        /*
        case ':':
            printf("OK");
            break;*/

        case '.':
            read_byte(lexer);

            key=get_token_data(lexer);

            array_push(operand, new_data(key, '.'));

            str_clear(lexer->token);
            //get_token(lexer->f, lexer->head, lexer->token);
            continue;

        case '(':
            args_expr=get_call_function_args(lexer);
            if(!args_expr)
                return 0;

            array_push(operand, new_data(args_expr, '('));
            break;

        case '[':
            read_byte(lexer);
            variable=lexer_get_expression(lexer);
            if(!variable)
                return 0;

            if(lexer->head!=']')
            {
                printf("expected ]\n");
                return 0;
            }
            read_byte(lexer);

            array_push(operand, new_data(variable, '['));
            break;
        }

        if(lexer->head!='.' && lexer->head!='(' && lexer->head!='[')
            return operand;

        str_clear(lexer->token);
        get_token(lexer, lexer->token);
    }
}

//----CALL OR ASSIGNMENT-------------------------------------------------------------

char get_call_or_assignment(Lexer *lexer)
{
    int         i;
    Array      *lvalue;
    List       *rvalue;
    Data       *first_function;
    Assignment *assignment;
    Call       *call;

    lvalue=get_defined_operand(lexer);
    if(!lvalue)
        return 0;

    skip(lexer);

    for(i=0; i<lvalue->length; i++)
    {
        first_function=lvalue->data[i];
        if(first_function->type=='(')
            break;
    }

    if(first_function->type=='(')
    {
        list_push(lexer->cur_body, new_data(lvalue, CALL));
    }
    else if(lexer->head=='=')
    {
        read_byte(lexer);
        skip(lexer);
        str_clear(lexer->token);

        rvalue=lexer_get_expression(lexer);
        if(!rvalue)
          return 0;

        assignment=new(Assignment);
        assignment->left_operand=lvalue;
        assignment->right_expression=rvalue;

        list_push(lexer->cur_body, new_data(assignment, ASSIGNMENT));
    }
    /*
    else
    {
        printf("expected =\n");
        return 0;
    }*/

    return 1;
}

//----BREAK--------------------------------------------------------------------------

char lexer_break(Lexer *lexer)
{
    list_push(lexer->cur_body, new_data(0, BREAK));
    return 1;
}

//----CONTINUE-----------------------------------------------------------------------

char lexer_continue(Lexer *lexer)
{
    list_push(lexer->cur_body, new_data(0, CONTINUE));
    return 1;
}

//----LOAD MODULE--------------------------------------------------------------------

static char get_next_object(Lexer *lexer, Function *class)
{
    String     *var_name=get_token_data(lexer);
    Assignment *assignment;
    Variable   *variable;

    //interpretator(lib_main);

    variable=new_static_variable(lexer, var_name, MODULE, class);

    if(!variable)
        return 0;

    return 1;
}

static char get_objects(Lexer *lexer, Function *class)
{
    do
    {
        read_byte(lexer);

        if(!get_next_object(lexer, class))
            return 0;

        skip(lexer);
    }
    while(lexer->head==',');

    return 1;
}

char lexer_load_class(Lexer *lexer)
{
    FILE     *f;
    Function *class;

    f=fopen(lexer->token->begin, "rb");

    if(!f)
        return 0;

    class=run_lexer(f, lexer->get_byte, lexer->end_data);
    interpretator(class);

    fclose(f);

    return get_objects(lexer, class);
}

//----INCLUDE------------------------------------------------------------------------

static char include_program(Lexer *lexer)
{
    String *include_name=get_token_data(lexer);
    FILE   *f=fopen(include_name->begin, "rb");

    if(!f)
    {
        printf("\nфайл ");
        str_print(include_name);
        printf(" не найден");

        return 0;
    }

    array_push(lexer->sources, lexer->source);

    lexer->source=f;
    //lexer->read_byte(lexer);
}

char lexer_include(Lexer *lexer)
{
    do
    {
        read_byte(lexer);

        if(!include_program(lexer))
            return 0;

        skip(lexer);
    }
    while(lexer->head==',');

    return 1;
}

//----RETURN-------------------------------------------------------------------------

char lexer_return(Lexer *lexer)
{
    List *return_expression=lexer_get_expression(lexer);
    list_push(lexer->cur_body, new_data(return_expression, RETURN));

    return 1;
}

static char new_push(Lexer *lexer, Array *array_operand)
{
    Push *push_data=new(Push);

    push_data->expression=lexer_get_expression(lexer);
    push_data->array=array_operand;

    if(!push_data->expression)
        return 0;

    list_push(lexer->cur_body, new_data(push_data, PUSH));

    return 1;
}

char lexer_push(Lexer *lexer)
{
    Array *array_operand;

    str_clear(lexer->token);
    skip(lexer);
    get_token(lexer, lexer->token);
    array_operand=get_defined_operand(lexer);

    if(!array_operand)
        return 0;

    new_push(lexer, array_operand);

    do
    {
        read_byte(lexer);
        new_push(lexer, array_operand);
    }
    while(lexer->head==',');

    return 1;
}

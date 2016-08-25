#include "variable.h"
#include "debug.h"


static Variable* create_variable(Parser *parser, String *name)
{
    Variable *variable=new(Variable);

    variable->name=name;
    tree_add(parser->cur_scope, variable);

    return variable;
}


Variable* new_variable(Parser *parser, String *name, char type)
{
    Variable *variable=create_variable(parser, name);

    variable->type      = type;
    variable->is_closed = 0;

    parser->cur_function->count_vars++;
    variable->shift = parser->cur_function->count_vars;

    return variable;
}


Variable* new_static_variable(Parser *parser, String *name, char type, int *data)
{
    Variable *variable=create_variable(parser, name);

    variable->type  = type;
    variable->shift = data;

    return variable;
}


static Variable* get_global_variable(String *name, Parser *parser)
{
    Variable  *finded_var;
    NodeStack *i;

    for(i=parser->scopes->begin; i; i=i->previouse)
    {
        finded_var=tree_find(i->data, name);

        if(finded_var)
        {
            finded_var->is_closed=1;
            return finded_var;
        }
    }

    return 0;
}


Variable* find_variable(String *name, Parser *parser)
{
    Variable  *finded_var;

    finded_var=tree_find(parser->cur_function->variables, name);

    if(!finded_var)
        finded_var=get_global_variable(name, parser);

    return finded_var;
}


static int variable_cmp(Variable *var1, Variable *var2)
{
    return str_comparision(var1->name, var2->name);
}


static int variable_str_var_cmp(String *s, Variable *var)
{
    return str_comparision(s, var->name);
}

static void variable_node_free(Variable *variable)
{

}


Function* new_function(String *name)
{
    Function *f=new(Function);

    f->name       = name;
    f->args       = array_init(8);
    f->body       = array_init(8);
    f->variables  = tree_init(variable_str_var_cmp, variable_cmp, variable_node_free);
    f->count_vars = 0;

    return f;
}


char parser_get_variable(Parser *parser, String *variable_name, char type)
{
    Assignment *assignment;
    Variable   *variable;
    Array      *expression;
    char        expression_type;

    if(debug)
    {
        printf("VAR ");
        str_print(variable_name);
    }

    variable=new_variable(parser, variable_name, type);
    skip(parser);

    if(parser->head!='=')
    {
        printf("\nнеопределённый токен ");
        str_print(variable_name);
        return 0;
    }

    if(debug)
        printf(" =");

    read_byte(parser);
    str_clear(parser->token);
    expression=parser_get_expression(parser);

    if(!expression)
        return 0;

    if(debug)
        printf("\n");
/*
    expression_type=infer_type(parser, expression->begin);

    if(expression_type==UNDEFINED)
        return 0;

    if(variable->type==UNDEFINED)
        variable->type=expression_type;
    else if(variable->type!=expression_type)
    {
        printf("\nтип переменной не может меняться");
        return 0;
    }*/

    //variable->type=infer_type(parser, expression);

    assignment=new(Assignment);
    assignment->left_operand     = array_init(1);
    assignment->right_expression = expression;

    array_push(assignment->left_operand, new_data(variable, 'v'));
    array_push(parser->cur_body, new_data(assignment, ASSIGNMENT));

    return 1;
}

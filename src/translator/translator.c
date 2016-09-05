#include "translator.h"
#include "../extends.h"
#include "../types.h"
#include "../Map.h"


extern int priorities[128];


typedef struct
{
    Data *operand1;
    char  operation;
    Data *operand2;
    char  is_in_brace;
}
BinaryOperator;


static Variable* eval_operand(Array *arr)
{
    Data       *data;

    Variable   *prev_var,
               *cur_var;
    int         i;
    Array      *index;

    Array      *array;
    Map        *map;
    Array      *expression;
    SystemCall *system_call;
    Function   *function;
    Variable   *system_variable;

    for(i=0; i<arr->length; i++)
    {
        data=arr->data[i];

        switch(data->type)
        {
            /*
        case 's':
            system_call=data->data;

            system_variable = interpretator->stack_head;

            system_variable->name  = interpretator_system_name;
            system_variable->type  = CONST_INTEGER;
            system_variable->shift = interpretator_system_call(system_call, interpretator);

            return system_variable;
            break;*/

        case 'v':
            cur_var=data->data;
            break;
/*
        case '[':
            index=data->data;

            if(cur_var->type!=ARRAY)
            {
                printf("\nvariable '");
                str_print(cur_var->name);
                printf("' is not array");

                return undefined;
            }

            array=cur_var->shift;

            cur_var=eval(interpretator, index->begin);

            switch(cur_var->type)
            {
            case CONST_INTEGER:
                if(cur_var->shift>=array->length)
                {
                    printf("\nвыход за границы массива");
                    return cur_var;
                }

                cur_var=array->data[cur_var->shift];
                break;
            }
            break;

        case '(':
            if(cur_var->type!=FUNCTION)
            {
                printf("\nvariable ");
                str_print(cur_var->name);
                printf(" is not function");

                return 0;
            }

            cur_var=run_function(interpretator, cur_var->shift, data->data);
            break;

        case '.':
            if(cur_var->type!=MAP)
            {
                if(cur_var->type!=MODULE)
                {
                    printf("\nеременная ");
                    str_print(cur_var->name);
                    printf(" не является ни множеством ни классом");

                    return 0;
                }

                function=cur_var->shift;
                cur_var=tree_find(function->variables, data->data);

                break;
            }

            map=cur_var->shift;

            cur_var=map_get(map, data->data);
            if(!cur_var)
            {
                printf("\nkey ");
                str_print(data->data);
                printf(" not found");

                return undefined;
            }
            break;*/
        }
    }

    return cur_var;
}


static void print_var(Variable *var)
{
    switch(var->type)
    {
    case CONST_INTEGER:
        printf("%d", var->shift);
        break;

    case STRING:
        printf("%s", var->shift);
        break;

    default:
        printf("error");
    }
}


static void print_operand(Data *operand)
{
    BinaryOperator *binary_operator;

    switch(operand->type)
    {
    case OPERAND:
        print_var(operand->data);
        break;

    case COMPOUND_OPERAND:
        print_var(eval_operand(operand->data));
        break;

    case EVALED_OPERAND:
        binary_operator=operand->data;

        if(binary_operator->is_in_brace)
            printf("(");

        print_operand(binary_operator->operand1);
        printf("%c", binary_operator->operation);
        print_operand(binary_operator->operand2);

        if(binary_operator->is_in_brace)
            printf(")");
        break;
    }
}


static void update_braces(Data *operand, unsigned char operation)
{
    if(operand->type!=EVALED_OPERAND)
        return;

    BinaryOperator *binary_operator=operand->data;

    if(priorities[(int)operation] > priorities[(int)binary_operator->operation])
        binary_operator->is_in_brace=1;
}


void convert_from_postfix_to_infix_notation(Array *expression)
{
    Variable       *var;
    Data           *data;
    char          **array_data=expression->data;
    int             length=expression->length;
    int             i;
    Array          *stack=array_init(10);
    Data           *operand1;
    Data           *operand2;
    BinaryOperator *binary_operator;

    for(i=0; i<length; i++)
    {
        data=array_data[i];

        switch(data->type)
        {
            case OPERAND:
            case COMPOUND_OPERAND:
                array_push(stack, data);
                //printf(" op");
                break;

            case OPERATION:
                //printf(" o");
                //printf("{%d}", stack->length);
                binary_operator=new(BinaryOperator);
                binary_operator->operand2    = array_pop(stack);
                binary_operator->operand1    = array_pop(stack);
                binary_operator->operation   = data->data;
                binary_operator->is_in_brace = 0;

                update_braces(binary_operator->operand1, binary_operator->operation);
                update_braces(binary_operator->operand2, binary_operator->operation);

                array_push(stack, new_data(binary_operator, EVALED_OPERAND));
                break;

            default:
                printf("\nошибка в преобразователе из постфиксной в инфиксную нотацию");
        }
    }

    print_operand(array_pop(stack));
}


#include "translator.h"
#include "get data.h"

#include "commands/push.h"
#include "commands/out.h"
#include "commands/state.h"


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


char parse_translator_state(TranslatorParser *parser, Translator *translator)
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

    get_byte_translator_parser(parser);
    parse_translator_body(parser, find_data_in_prefix_tree(translator_states, state_name));
    get_byte_translator_parser(parser);

    return 1;
}


TranslatorParser* parse_translator(char  *in_source,
                                   char (*get_byte)(char *source),
                                   char (*end_of_data)(char *source),
                                   char  *out_source,
                                   void (*out_byte)(char data, char *source))
{
    TranslatorParser *parser     = new(TranslatorParser);
    Translator       *translator = new(Translator);

    parser->end_of_data = end_of_data;
    parser->get_byte    = get_byte;
    parser->in_source   = in_source;
    parser->end_data    = 0;
    parser->out_source  = out_source;
    parser->out_byte    = out_byte;
    parser->token       = str_init("");

    translator->states = create_prefix_tree();

    get_byte_translator_parser(parser);

    while(!parser->end_data)
    {
        if(!parse_translator_state(parser, translator))
            return 0;
    }

    return translator;
}

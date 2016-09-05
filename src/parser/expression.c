#include "expression.h"
#include "operand.h"
#include "../Map.h"
#include "../GetData.h"
#include "../translator/translator.h"


typedef struct
{
    char          *operation;
    unsigned char  number_operation;
}
Operation;


static int       length_condition_operations=2;
static Operation condition_operations[2];
static int       length_condition_unary_operation=1;
static Operation condition_unary_operations[1];

static int       length_expression_operations=14;
static Operation expression_operations[14];
static int       length_expression_unary_operation=2;
static Operation expression_unary_operations[2];

int              priorities[128];


void init_expression()
{
    expression_unary_operations[0].operation="-";
    expression_unary_operations[0].number_operation='_';

    expression_unary_operations[1].operation="~";
    expression_unary_operations[1].number_operation='~';
/*
    expression_unary_operations[2].operation="!";
    expression_unary_operations[2].number_operation='_';
*/
    expression_operations[0].operation="|";
    expression_operations[0].number_operation='|';

    expression_operations[1].operation="^";
    expression_operations[1].number_operation='^';

    expression_operations[2].operation="&";
    expression_operations[2].number_operation='&';

    expression_operations[3].operation="=";
    expression_operations[3].number_operation='=';

    expression_operations[4].operation="!=";
    expression_operations[4].number_operation='!';

    expression_operations[5].operation="<";
    expression_operations[5].number_operation='<';

    expression_operations[6].operation=">";
    expression_operations[6].number_operation='>';

    expression_operations[7].operation="<=";
    expression_operations[7].number_operation='1';

    expression_operations[8].operation=">=";
    expression_operations[8].number_operation='2';

    expression_operations[9].operation="+";
    expression_operations[9].number_operation='+';

    expression_operations[10].operation="-";
    expression_operations[10].number_operation='-';

    expression_operations[11].operation="%";
    expression_operations[11].number_operation='%';

    expression_operations[12].operation="*";
    expression_operations[12].number_operation='*';

    expression_operations[13].operation="/";
    expression_operations[13].number_operation='/';


    priorities['(']=0;
    priorities['|']=1;
    priorities['^']=2;
    priorities['&']=3;

    priorities['=']=4; // ==
    priorities['!']=4; // !=

    priorities['<']=5; // <
    priorities['>']=5; // >
    priorities['1']=5; // <=
    priorities['2']=5; // >=

    priorities['+']=6;
    priorities['-']=6;
    priorities['%']=7;
    priorities['*']=7;
    priorities['/']=7;

    priorities['~']=8;
    priorities['_']=9; //_ - унарный -


    condition_unary_operations[0].operation="not";
    condition_unary_operations[0].number_operation='n';

    condition_operations[0].operation="or";
    condition_operations[0].number_operation='o';

    condition_operations[1].operation="and";
    condition_operations[1].number_operation='a';


    priorities['o']=1; //boolean or
    priorities['n']=2; //boolean not
    priorities['a']=2; //boolean and
}


static char stack_contains_open_gap(Array *stack)
{
    int    i=stack->length-1;
    char **data=stack->data;

    for(; i>=0; i--)
        if(data[i]=='(')
            return 1;

    return 0;
}


static void add_operation(Array *expression, Array *stack, int operation, int *prior)
{
    if(array_empty(stack) || prior[(int)array_top(stack)] < prior[operation])
        array_push(stack, operation);
    else
    {
        while(!array_empty(stack) && prior[(int)array_top(stack)] >= prior[operation])
            array_push(expression, new_data(array_pop(stack), OPERATION));

        array_push(stack, operation);
    }
}


static char get_close_brace(Array *stack, Array *expression)
{
    if(array_top(stack)=='(')
    {
        printf("\nскобки без выражения излишни");
        return 0;
    }

    do
    {
        if(array_empty(stack))
        {
            printf("\nотсутствует (");
            return 0;
        }

        array_push(expression, new_data(array_pop(stack), OPERATION));
    }
    while(array_top(stack)!='(');

    array_pop(stack);

    return 1;
}


static char get_expression_operand(Parser *parser, Array *expression)
{
    if(!get_operand(parser, expression))
        return 0;

    return 1;
}


Array* get_expression_in_postfix_notation(Parser *parser,
                                          char (*get_operand_function)(Parser *parser, Array *expression),
                                          Operation *unary_operations, int length_unary_operations,
                                          Operation *binary_operations, int length_binary_operations)
{
    Array    *stack                 = array_init(4);
    Array    *expression            = array_init(8);

    char      is_expression         = 1,
              is_operation          = 0,
              is_close_gap          = 0,
              op;

    char      is_unary_operation;
    int       i;

    while(!parser->end_of_data && is_expression)
    {
        is_unary_operation=1;

        if(!is_close_gap)
        {
            do
            {
                is_unary_operation=0;
                skip(parser);

                for(i=0; i<length_unary_operations; i++)
                {
                    if(is_true_word(parser, unary_operations[i].operation))
                    {
                        add_operation(expression, stack, unary_operations[i].number_operation, priorities);
                        is_unary_operation=1;
                        break;
                    }

                    skip(parser);
                }
            }
            while(is_unary_operation);
        }

        if(parser->head!='(' && !is_close_gap)
        {
            if(!get_operand_function(parser, expression))
                return 0;

            is_operation=0;
        }

        switch(parser->head)
        {
        case '(':
            array_push(stack, '(');
            read_byte(parser);

            is_operation=1;
            is_close_gap=0;
            break;

        case ')':
            if(!stack_contains_open_gap(stack))
            {
                is_expression=0;
                break;
            }

            get_close_brace(stack, expression);
            read_byte(parser);

            is_operation=0;
            is_close_gap=1;
            break;

        default:
            is_expression=0;

            for(i=0; i<length_binary_operations; i++)
            {
                if(is_true_word(parser, binary_operations[i].operation))
                {
                    if(is_operation)
                    {
                        printf("2 операции без операнда\n");
                        return 0;
                    }

                    add_operation(expression, stack, binary_operations[i].number_operation, priorities);

                    is_operation=1;
                    is_close_gap=0;
                    is_expression=1;

                    break;
                }
            }
        }

        skip(parser);
        str_clear(parser->expr_token);
    }

    while(!array_empty(stack))
    {
        op=array_pop(stack);

        if(op=='(')
        {
            printf("expected )\n");
            return 0;
        }

        array_push(expression, new_data(op, OPERATION));
    }

    if(!expression->length)
    {
        printf("\nотсутствует выражение");
        return 0;
    }

    return expression;
}


Array* parser_get_expression(Parser *parser)
{
    /*
    return get_expression_in_postfix_notation(parser,
                                              get_expression_operand,
                                              expression_unary_operations, length_expression_unary_operation,
                                              expression_operations, length_expression_operations);*/
    Array *expression=get_expression_in_postfix_notation(parser,
                                                         get_expression_operand,
                                                         expression_unary_operations, length_expression_unary_operation,
                                                         expression_operations, length_expression_operations);
    if(expression)
        convert_from_postfix_to_infix_notation(expression);

    return expression;
}


static char get_condition_operand(Parser *parser, Array *condition)
{
    Array *arithmetic_expression=parser_get_expression(parser);

    if(!arithmetic_expression)
        return 0;

    array_push(condition, new_data(arithmetic_expression, OPERAND));

    return 1;
}


Array* parser_get_condition(Parser *parser)
{
    return get_expression_in_postfix_notation(parser,
                                              get_condition_operand,
                                              condition_unary_operations, length_condition_unary_operation,
                                              condition_operations, length_condition_operations);

}

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

static int       length_expression_operations=14;
static Operation expression_operations[14];

int              priorities[128];


void init_expression()
{
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
    priorities['_']=9; //_ - ������� -


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
        printf("\n������ ��� ��������� �������");
        return 0;
    }

    do
    {
        if(array_empty(stack))
        {
            printf("\n����������� (");
            return 0;
        }

        array_push(expression, new_data(array_pop(stack), OPERATION));
    }
    while(array_top(stack)!='(');

    array_pop(stack);

    return 1;
}


Array* parser_get_expression(Parser *parser)
{
    Array    *stack                 = array_init(4);
    Array    *expression            = array_init(8);

    char      is_expression         = 1,
              is_operation          = 0,
              is_close_gap          = 0,
              op;
    int       i;

    while(!parser->end_of_data && is_expression)
    {
        while(parser->head=='-' || parser->head=='~' || parser->head=='!')
        {
            if(is_true_word(parser, "!="))
            {
                printf("\n�������� != ��� ��������, �������� ������� ! (��������� \"��\")");
                return 0;
            }

            read_byte(parser);
            skip(parser);

            if(parser->head=='-')
                add_operation(expression, stack, '_', priorities);
            else
                add_operation(expression, stack, parser->head, priorities);
        }

        skip(parser);

        if(parser->head!='(' && !is_close_gap)
        {
            if(!get_operand(parser, expression))
                return 0;

            if(parser->expr_token->length)
                break;

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

            for(i=0; i<length_expression_operations; i++)
            {
                if(is_true_word(parser, expression_operations[i].operation))
                {
                    if(is_operation)
                    {
                        printf("2 �������� ��� ��������\n");
                        return 0;
                    }

                    add_operation(expression, stack, expression_operations[i].number_operation, priorities);

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
        printf("\n����������� ���������");
        return 0;
    }

    convert_from_postfix_to_infix_notation(expression);

    return expression;
}


Array* parser_get_condition(Parser *parser)
{
    Variable *operand;
    Array    *stack                  = array_init(2);
    Array    *condition              = array_init(4),
             *arithmetic_expression;

    char      is_condition           = 1,
              is_operation           = 0,
              is_close_gap           = 0,
              op;

    int       i;

    while(!parser->end_of_data && is_condition)
    {
        skip(parser);

        while(is_true_word(parser, "not"))
        {
            add_operation(condition, stack, 'n', priorities);
            skip(parser);
        }

        if(parser->head!='(' && !is_close_gap)
        {
            arithmetic_expression=parser_get_expression(parser);

            if(!arithmetic_expression)
                return 0;

            array_push(condition, new_data(arithmetic_expression, OPERAND));

            is_operation=0;
        }

        switch(parser->head)
        {
        case '(':
            array_push(stack, '(');
            is_operation=1;
            is_close_gap=0;
            break;

        case ')':
            get_close_brace(stack, condition);
            is_operation=0;
            is_close_gap=1;
            break;

        default:
            is_condition=0;

            for(i=0; i<length_condition_operations; i++)
            {
                if(is_true_word(parser, condition_operations[i].operation))
                {
                    if(is_operation)
                    {
                        printf("2 �������� ��� ��������\n");
                        return 0;
                    }

                    add_operation(condition, stack, condition_operations[i].number_operation, priorities);

                    is_operation=1;
                    is_close_gap=0;
                    is_condition=1;

                    break;
                }
            }
        }

        if(is_condition)
            read_byte(parser);
    }

    while(!array_empty(stack))
    {
        op=array_pop(stack);

        if(op=='(')
        {
            printf("expected )\n");
            return 0;
        }

        array_push(condition, new_data(op, OPERATION));
    }

    if(!condition->length)
        return 0;

    return condition;
}

#include "EvalCondition.h"
#include "../extends.h"

#define EVALUATED_CONDITION 0
#define NOT_EVALUATED_CONDITION 1


static void push_boolean_expression(Interpretator *interpretator, char *boolean_expression, char type)
{
    Data *data=interpretator->stack_head;

    data->data=boolean_expression;
    data->type=type;

    interpretator->stack_head=(Data*)interpretator->stack_head+1;
}


static Data* pop_boolean_expression(Interpretator *interpretator)
{
    interpretator->stack_head=(Data*)interpretator->stack_head-1;
    return interpretator->stack_head;
}


char eval_condition(Interpretator *interpretator, struct ListNode *i)
{
    Variable *rez;
    Data     *data;

    List     *cond1;
    Data     *data1;
    char      type1;

    List     *cond2;
    Data     *data2;
    char      type2;

    for(; i; i=i->next)
    {
        data=i->data;

        switch(data->type)
        {
            case OPERAND:
                push_boolean_expression(interpretator, data->data, NOT_EVALUATED_CONDITION);
                break;

            case OPERATION:
                switch((int)data->data)
                {
                case 'a':
                    data2=pop_boolean_expression(interpretator);
                    cond2=data2->data;
                    type2=data2->type;

                    data1=pop_boolean_expression(interpretator);
                    cond1=data1->data;
                    type1=data1->type;

                    if(type1==NOT_EVALUATED_CONDITION)
                    {
                        eval(interpretator, cond1->begin);
                        rez=interpretator_pop_var(interpretator);

                        if(!rez->shift)
                        {
                            push_boolean_expression(interpretator, 0, EVALUATED_CONDITION);
                            break;
                        }
                    }
                    else if(!cond1)
                    {
                        push_boolean_expression(interpretator, 0, EVALUATED_CONDITION);
                        break;
                    }

                    if(type2==NOT_EVALUATED_CONDITION)
                    {
                        eval(interpretator, cond2->begin);
                        rez=interpretator_pop_var(interpretator);

                        push_boolean_expression(interpretator, rez->shift, EVALUATED_CONDITION);
                    }
                    else
                        push_boolean_expression(interpretator, cond2, EVALUATED_CONDITION);

                    break;

                case 'o':
                    break;

                case 'n':
                    break;
                }

                break;
        }
    }

    data1=pop_boolean_expression(interpretator);
    cond1=data1->data;
    type1=data1->type;

    if(type1==NOT_EVALUATED_CONDITION)
    {
        eval(interpretator, cond1->begin);
        rez=interpretator_pop_var(interpretator);

        //printf("\nboolean %d", rez->shift);
        return rez->shift;
    }
    else
    {
        //printf("\nboolean %d", cond1);
        return cond1;
    }
}

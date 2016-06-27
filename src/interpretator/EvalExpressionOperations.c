#include "EvalExpressionOperations.h"


static char* get_left_variable_data(Interpretator *interpretator, Variable *variable, Variable *variable2)
{
    switch(variable->type)
    {
    case CONST_REAL:
        return &variable->shift;
        break;

    case CONST_INTEGER:
        return &variable->shift;

    default:
        printf("\n���� �� ��������� ��� ��� ���������� ����� ���������� ����� ���� %d", variable->type);
        break;
    }
}


void add(Interpretator *interpretator)
{
    Variable *op2=interpretator_pop_var(interpretator),
             *op1=interpretator->stack_head-1;

    switch(op1->type)
    {
    case CONST_INTEGER:
        switch(op2->type)
        {
        case CONST_INTEGER:
            op1->shift+=op2->shift;
            break;

        case CONST_REAL:
            op1->type=CONST_REAL;
            *(float*)&op1->shift=(float)op1->shift;

            *(float*)&op1->shift+=*(float*)&op2->shift;
            break;
        }
        break;

    case CONST_REAL:
        switch(op2->type)
        {
        case CONST_INTEGER:
            op2->type=CONST_REAL;
            *(float*)&op2->shift=(float)op2->shift;

            *(float*)&op1->shift+=*(float*)&op2->shift;
            break;

        case CONST_REAL:
            *(float*)&op1->shift+=*(float*)&op2->shift;
            break;
        }
        break;
    }
}


void sub(Interpretator *interpretator)
{
    Variable *op2=interpretator_pop_var(interpretator),
             *op1=interpretator->stack_head-1;
    char     *left_variable_data=get_left_variable_data(interpretator, interpretator->stack_head-1, op2);

    switch(op2->type)
    {
    case CONST_INTEGER:
        *(int*)left_variable_data-=op2->shift;
        break;

    default:
        printf("\n���� �� ��������� ��� ��� ���������� ������ ���������� � ���������");
        break;
    }
}


void mul(Interpretator *interpretator)
{
    Variable *op2=interpretator_pop_var(interpretator),
             *op1=interpretator->stack_head-1;
    char     *left_variable_data=get_left_variable_data(interpretator, interpretator->stack_head-1, op2);

    switch(op2->type)
    {
    case CONST_INTEGER:
        *(int*)left_variable_data*=op2->shift;
        break;

    default:
        printf("\n���� �� ��������� ��� ��� ���������� ������ ���������� � ���������");
        break;
    }
}


void division(Interpretator *interpretator)
{
    Variable *op2=interpretator_pop_var(interpretator),
             *op1=interpretator->stack_head-1;
    char     *left_variable_data=get_left_variable_data(interpretator, interpretator->stack_head-1, op2);

    switch(op2->type)
    {
    case CONST_INTEGER:
        *(int*)left_variable_data/=op2->shift;
        break;

    default:
        printf("\n���� �� ��������� ��� ��� ���������� ������ ���������� � �������");
        break;
    }
}


void not_equal(Interpretator *interpretator)
{
    Variable *op2 = interpretator_pop_var(interpretator);
    char     *left_variable_data = get_left_variable_data(interpretator, interpretator->stack_head-1, op2);

    switch(op2->type)
    {
    case CONST_INTEGER:
        *(int*)left_variable_data = *(int*)left_variable_data != op2->shift;
        break;

    default:
        printf("\n���� �� ��������� ��� ��� ���������� ������ ���������� � �����");
        break;
    }
}


void equal(Interpretator *interpretator)
{
    Variable *op2 = interpretator_pop_var(interpretator);
    char     *left_variable_data = get_left_variable_data(interpretator, interpretator->stack_head-1, op2);

    switch(op2->type)
    {
    case CONST_INTEGER:
        *(int*)left_variable_data = *(int*)left_variable_data == op2->shift;
        break;

    default:
        printf("\n���� �� ��������� ��� ��� ���������� ������ ���������� � �����");
        break;
    }
}


void lesser(Interpretator *interpretator)
{
    Variable *op2 = interpretator_pop_var(interpretator);
    char     *left_variable_data = get_left_variable_data(interpretator, interpretator->stack_head-1, op2);

    switch(op2->type)
    {
    case CONST_INTEGER:
        *(int*)left_variable_data = *(int*)left_variable_data < op2->shift;
        break;

    default:
        printf("\n���� �� ��������� ��� ��� ���������� ������ ���������� � ������");
        break;
    }
}


void lesser_or_equal(Interpretator *interpretator)
{
    Variable *op2 = interpretator_pop_var(interpretator);
    char     *left_variable_data = get_left_variable_data(interpretator, interpretator->stack_head-1, op2);

    switch(op2->type)
    {
    case CONST_INTEGER:
        *(int*)left_variable_data = *(int*)left_variable_data <= op2->shift;
        break;

    default:
        printf("\n���� �� ��������� ��� ��� ���������� ������ ���������� � ������ ���� �����");
        break;
    }
}


void greather(Interpretator *interpretator)
{
    Variable *op2 = interpretator_pop_var(interpretator);
    char     *left_variable_data = get_left_variable_data(interpretator, interpretator->stack_head-1, op2);

    switch(op2->type)
    {
    case CONST_INTEGER:
        *(int*)left_variable_data = *(int*)left_variable_data > op2->shift;
        break;

    default:
        printf("\n���� �� ��������� ��� ��� ���������� ������ ���������� � ������");
        break;
    }
}


void greather_or_equal(Interpretator *interpretator)
{
    Variable *op2 = interpretator_pop_var(interpretator);
    char     *left_variable_data = get_left_variable_data(interpretator, interpretator->stack_head-1, op2);

    switch(op2->type)
    {
    case CONST_INTEGER:
        *(int*)left_variable_data = *(int*)left_variable_data >= op2->shift;
        break;

    default:
        printf("\n���� �� ��������� ��� ��� ���������� ������ ���������� � ������ ���� �����");
        break;
    }
}


void and(Interpretator *interpretator)
{
    Variable *op2=interpretator_pop_var(interpretator);
    char     *left_variable_data=get_left_variable_data(interpretator, interpretator->stack_head-1, op2);

    switch(op2->type)
    {
    case CONST_INTEGER:
        *(int*)left_variable_data=*(int*)left_variable_data<op2->shift;
        break;

    default:
        printf("\n���� �� ��������� ��� ��� ���������� ������ ���������� � ������");
        break;
    }
}


void unary_negate(Interpretator *interpretator)
{

}

#include "interpretator_operations.h"
#include "../Array.h"
#include "../Map.h"
#include "EvalExpression.h"
#include "EvalCondition.h"
#include "memory.h"
#include "../SystemLibrary.h"


static Variable* eval_left_operand(Array *operands, Interpretator *interpretator, char new_type, Variable *right_variable)
{
    Data           *data;

    Variable       *prev_var,
                   *cur_var,
                   *variable;
    int             i,
                    index;
    Array          *index_expression;

    Array          *array;
    Map            *map;
    Array          *expression;
    Array          *args;
    int             j;
    Function       *function;
    int           (*system_function)();
    SystemLibrary   system_library;
    String         *string;
    Variable        temp_type;

    for(i=0; i<operands->length; i++)
    {
        data=operands->data[i];

        switch(data->type)
        {
        case 'v':
            cur_var=data->data;

            if(cur_var->type==ARGUMENT)
                cur_var=interpretator->stack_base[cur_var->shift];
            break;

        case '[':
            index_expression=data->data;

            if(cur_var->type!=ARRAY)
            {
                printf("\nпеременная '");
                str_print(cur_var->name);
                printf("' не является массивом");

                return 0;
            }

            array=cur_var->shift;

            eval(interpretator, index_expression);
            cur_var=interpretator_pop_var(interpretator);

            switch(cur_var->type)
            {
            case CONST_INTEGER:
                if(cur_var->shift>=array->length)
                {
                    printf("\nout of range in array");
                    return cur_var;
                }

                cur_var=array->data[cur_var->shift];

                break;
            }

            break;

        case '(':
            args=data->data;

            switch(cur_var->type)
            {
            case FUNCTION:
                cur_var=run_function(interpretator, cur_var->shift, data->data);
                break;

            case SYSTEM_FUNCTION:
                {
                    int             new_args[args->length];
                    int             i;
                    Variable       *arg;
                    Array          *list;

                    for(i=0; i<args->length; i++)
                    {
                        list=args->data[i];
                        eval(interpretator, list);
                        arg=interpretator_pop_var(interpretator);

                        switch(arg->type)
                        {
                        case CONST_INTEGER:
                            //printf("INTEGER %d", arg->shift);
                            new_args[i]=arg->shift;
                            break;

                        case CONST_REAL:
                            new_args[i]=arg->shift;
                            //printf("{%f}", *(float*)&arg->shift);
                            break;

                        case STRING:
                            {
                            String *string=arg->shift;
                            //printf("STRING %s", string->begin);
                            new_args[i]=string->begin;
                            break;
                            }
                        }
                    }

                    system_function=cur_var->shift;
                    printf("\nSystem function return %d", run_system_function(system_function, new_args, args->length));
                }
                break;

            default:
                printf("\nvariable ");
                str_print(cur_var->name);
                printf(" is not function");

                return 0;
            }

            break;

        case '.':

            switch(cur_var->type)
            {
            case MAP:
                map=cur_var->shift;

                cur_var=map_get(map, data->data);
                if(!cur_var)
                {
                    map_add(map, data->data, undefined);
                    cur_var=map_get(map, data->data);
                }

                if(i==operands->length-1)
                {
                    cur_var->type=new_type;
                    return &cur_var->shift;
                }
                break;

            case MODULE:
                function=cur_var->shift;
                cur_var=tree_find(function->variables, data->data);
                //str_print(cur_var->name);
                break;

            default:
                printf("\nпеременная ");
                str_print(cur_var->name);
                printf(" не является ни множеством ни классом");

                return undefined;
            }

            break;
        }
    }

    switch(new_type)
    {
        case INTEGER:
        case CONST_INTEGER:
            cur_var->type=INTEGER;
            break;

        case STRING:
            cur_var->type=STRING;
            return &cur_var->shift;
            break;

        case ARRAY:
            cur_var->type=ARRAY;
            return &cur_var->shift;
            break;

        case FUNCTION:
            cur_var->type=FUNCTION;
            return &cur_var->shift;
            break;

        case MAP:
            cur_var->type=MAP;
            return &cur_var->shift;
            break;

        default:
            printf("\nне определён тип при вычислении правой переменной в присваивании");
    }

    if(cur_var->is_closed)
    {

    }
    else
    {
        /*
        switch(cur_var->type)
        {
        case INTEGER:
            return (int*)inte
            break;
        }*/

        return interpretator->stack_base+cur_var->shift;
    }
}


char interpretator_assignment(Assignment *assignment_data, Interpretator *interpretator)
{
    Variable   *result;
    char       *left_variable;

    eval(interpretator, assignment_data->right_expression);
    result=interpretator_pop_var(interpretator);

    left_variable=eval_left_operand(assignment_data->left_operand, interpretator, result->type, result);

    switch(result->type)
    {
    case CONST_REAL:
        *(float*)left_variable=*(float*)&result->shift;
        break;

    case CONST_INTEGER:

        *(int*)left_variable=result->shift;
        break;

    case GC_INTEGER:
        printf("ASSIGNMENT: OPA");
        break;

    case ARRAY:
        *(int*)left_variable=result->shift;
        break;

    case FUNCTION:
        *(int*)left_variable=result->shift;
        break;

    case MAP:
        *(int*)left_variable=result->shift;
        break;

    case STRING:
        *(int*)left_variable=result->shift;
        break;

    default:
        printf("ASSIGNMENT: OPA %d", result->type);
        break;
    }

    return LOOP_NORMAL;
}


char interpretator_loop(Array *loop_body, Interpretator *interpretator)
{
    while(execute(interpretator, loop_body)!=LOOP_BREAK);

    return LOOP_NORMAL;
}


char interpretator_break()
{
    return LOOP_BREAK;
}


char interpretator_continue()
{
    return LOOP_CONTINUE;
}


char interpretator_call(Array *call_data, Interpretator *interpretator)
{
    char     *result;

    result=eval_left_operand(call_data, interpretator, FUNCTION, 0);

    return LOOP_NORMAL;
}


char interpretator_system_call(SystemCall *system_call_data, Interpretator *interpretator)
{
    Array          *args                    = system_call_data->args;
    int             new_args[args->length];
    int             i;
    Variable       *arg;
    Array          *list;
    String         *string;

    for(i=0; i<args->length; i++)
    {
        list=args->data[i];
        eval(interpretator, list);
        arg=interpretator_pop_var(interpretator);

        switch(arg->type)
        {
        case CONST_INTEGER:
            new_args[i]=arg->shift;
            break;

        case CONST_REAL:
            new_args[i]=arg->shift;
            break;

        case STRING:
            string=arg->shift;
            new_args[i]=string->begin;
            break;
        }
    }

    run_system_function(system_call_data->function, new_args, args->length);
}


static void print_map_node(MapNode *map_node)
{
    printf("\n");
    str_print(map_node->key);
}


static void print_variable(Interpretator *interpretator, Variable *result)
{
    GCVariable *gc_variable;
    int         i;
    Array      *array;
    Array      *expression;
    Map        *map;
    //printf("\n^>%d", result);

    if(!result)
    {
        printf("\nошибка при печати-переменная не проинициализирована");
        return;
    }

    switch(result->type)
    {
    case UNDEFINED:
        printf("undefined");
        break;

    case STRING:
        str_print(result->shift);
        break;

    case INTEGER:
        printf("%d", *((int*)interpretator->stack_base+result->shift));
        break;

    case CONST_INTEGER:
        printf("%d", result->shift);
        break;

    case GC_INTEGER:
        gc_variable=result->shift;
        printf("%d", gc_variable->data);
        break;

    case ARRAY:
        array=result->shift;

        printf("[");
        for(i=0; i<array->length-1; i++)
        {
            print_variable(interpretator, array->data[i]);
            printf(", ");
        }

        if(array->length)
            print_variable(interpretator, array->data[array->length-1]);

        printf("]");
        break;

    case FUNCTION:
        printf("\nfunction ");
        str_print(result->name);
        printf(" 0x%08x", result->shift);
        break;

    case MAP:
        map=result->shift;

        printf("\n{");
        tree_print(map->tree, print_map_node);
        printf("\n}");
        break;

    case CONST_REAL:
        printf("%f", *(float*)&result->shift);
        break;
    }
}


char interpretator_print(Array *expression, Interpretator *interpretator)
{
    Variable *print_var;

    print_var=eval(interpretator, expression);
    interpretator_pop_var(interpretator);

    print_variable(interpretator, print_var);

    return LOOP_NORMAL;
}


char interpretator_push(Push *push_data, Interpretator *interpretator)
{
    Variable *array_variable=eval_operand(push_data->array, interpretator),
             *array_element;
    Array    *array;

    if(array_variable->type!=ARRAY)
    {
        printf("\npush работает только с массивами");
        return LOOP_NORMAL;
    }

    array=array_variable->shift;
    array_element=interpretator_heap_var_alloc(interpretator, eval(interpretator, push_data->expression));

    array_push(array, array_element);

    return LOOP_NORMAL;
}


char interpretator_if(If *if_data, Interpretator *interpretator)
{
    printf("\n<if>");

    if(eval_condition(interpretator, if_data->condition))
        return execute(interpretator, if_data->body);
    else
        return execute(interpretator, if_data->else_body);

    return LOOP_NORMAL;
}


char interpretator_while(While *while_data, Interpretator *interpretator)
{
    printf("\n<while>");
    while(eval_condition(interpretator, while_data->condition) && execute(interpretator, while_data->body)!=LOOP_BREAK) ;

    return LOOP_NORMAL;
}


char interpretator_do(Do *do_data, Interpretator *intepretator)
{
    /*
    if(execute(interpretator, do_data->body)!=LOOP_BREAK)
        while(do_data->condition && execute(interpretator, do_data->body)!=LOOP_BREAK);
*/
    return LOOP_NORMAL;
}

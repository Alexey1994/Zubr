#include "array.h"
#include "extends.h"


Array* array_init(int reserve)
{
    Array *array=new(Array);

    array->reserve       = reserve;
    array->buffer_length = reserve;
    array->data          = new_array(char*, reserve);
    array->length        = 0;

    return array;
}


void array_push(Array *array, char *data)
{
    if(array->length==array->buffer_length)
    {
        array->buffer_length+=array->reserve;
        reallocate_array(array->data, char*, array->buffer_length);
    }

    array->data[array->length]=data;
    array->length++;
}


char* array_pop(Array *array)
{
    array->length--;
    return array->data[array->length];
}


void array_clear(Array *array)
{
    array->length=0;
}


char* array_top(Array *array)
{
    return array->data[array->length-1];
}


char array_empty(Array *array)
{
    return !array->length;
}


void array_free(Array *array)
{
    free(array->data);
    free(array);
}

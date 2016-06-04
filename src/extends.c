#include "extends.h"
#include "String.h"
#include <stdio.h>

Data* new_data(char *data, char type)
{
    Data *data_alloc=new(Data);

    data_alloc->data=data;
    data_alloc->type=type;

    return data_alloc;
}

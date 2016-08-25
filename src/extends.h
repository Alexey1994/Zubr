#ifndef EXTENDS_H_INCLUDED
#define EXTENDS_H_INCLUDED

#include "String.h"
#include <stdlib.h>

#define new(data_type) malloc(sizeof(data_type))
#define new_array(type, length) malloc(sizeof(type)*length)

#define reallocate(data, type) data=realloc(data, sizeof(type))
#define reallocate_array(data, type, length) data=realloc(data, sizeof(type)*length)

#define loop for(;;)

#define OPERATION        1
#define OPERAND          2
#define COMPOUND_OPERAND 3
//#define EXPRESSION       4
#define EVALED_OPERAND   5

typedef struct
{
    char *data;
    char  type;
}
Data;

#endif // EXTENDS_H_INCLUDED

#ifndef EXTENDS_H_INCLUDED
#define EXTENDS_H_INCLUDED

#include "String.h"

#define new(data_type) malloc(sizeof(data_type))
#define arr_alloc(data_type, sz) malloc(sizeof(#data_type)*(#sz))

#define loop for(;;)

#define OPERATION        1
#define OPERAND          2
#define COMPOUND_OPERAND 3
#define EXPRESSION       4

typedef struct
{
    char *data;
    char  type;
}
Data;

#endif // EXTENDS_H_INCLUDED

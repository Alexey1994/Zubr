#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include "String.h"
#include "tree.h"
#include "stack.h"
#include "Array.h"

#define UNDEFINED        0x00
#define STRING           0x01
#define INTEGER          0x02
#define CONST_INTEGER    0x03
#define GC_INTEGER       0x04
#define ARRAY            0x05
#define MAP              0x06
#define REAL             0x07
#define CONST_REAL       0x08
#define GC_REAL          0x09
#define BOOLEAN          0x0a
#define FUNCTION         0x0b
#define MODULE           0x0c
#define ARGUMENT         0x0d
#define NUMERIC_ARGUMENT 0x0e
#define SYSTEM_FUNCTION  0x0f

#define END           0x01
#define PRINT         0x02
#define IF            0x03
#define WHILE         0x04
#define LOOP          0x05
#define BREAK         0x06
#define CONTINUE      0x07
#define ELSE          0x08
#define DO            0x09
#define CALL          0x0a
#define ASSIGNMENT    0x0b
#define PUSH          0x0c
#define SYSTEM_CALL   0x0d

typedef struct
{
    Array *args;
    char (*function)();
}
SystemCall;

typedef struct
{
    char    is_closed;
    char    type;
    String *name;
    int     shift;
}
Variable;

typedef struct
{
    String    *name;
    Tree      *functions;
    Array     *body;
    Tree      *bodies;
    Array     *args;
    Variable  *return_var;
    Tree      *variables;
    int        count_vars;
}
Function;

typedef struct
{
    Function  *main_function;
    //Tree      *variables;
    Variable **variables;
    //int        current_variable;
}
Module;

typedef struct
{
    Module *module;
    int     current_variable;
}
ModuleElement;


typedef struct
{
    Array  *condition;
    Array  *body;
    Array  *else_body;
}
If;

typedef struct
{
    Array *condition;
    Array *body;
}
While;

typedef struct
{
    Array *condition;
    Array *body;
}
Do;

typedef struct
{
    Variable *function;
    Array    *args;
}
Call;

typedef struct
{
    Array *left_operand;
    Array *right_expression;
}
Assignment;

typedef struct
{
    Array *array;
    Array *expression;
}
Push;

extern Variable *undefined;

#endif // TYPES_H_INCLUDED

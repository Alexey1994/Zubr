#ifndef SYSTEM_LIBRARY_H_INCLUDED
#define SYSTEM_LIBRARY_H_INCLUDED

#include "String.h"

#define add_system_function_integer_argument(arg) __asm__("sub   $4, %%esp\n" "movl  %0, (%%esp)" : :"r"(arg));


typedef struct
{
    String *name;
    char   *library;
}SystemLibrary;


SystemLibrary* open_system_library(String *name);
void close_system_library(SystemLibrary *library);
char* get_system_function(SystemLibrary *library, String *name);
int run_system_function(int(*f)(), int *args, int length);

void add_system_function_real_argument(float arg);
//void add_system_function_integer_argument(int arg);

#endif // SYSTEM_LIBRARY_H_INCLUDED

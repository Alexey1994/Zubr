#include "SystemLibrary.h"
#include "extends.h"


static char *LIBRARY_PATH="libs/";
static int   SIZE_OF_LIBRARY_PATH=5;

//#ifdef WIN32
#include <windows.h>

static void copy(char *s1, char *s2, int length)
{
    for(; length; length--, s1++, s2++)
        *s1=*s2;
}

SystemLibrary* open_system_library(String *name)
{
    SystemLibrary *library;
    char          *loaded_library;
    char           path[SIZE_OF_LIBRARY_PATH + name->length+1];

    strcpy(path, LIBRARY_PATH);
    strcpy(path+SIZE_OF_LIBRARY_PATH, name->begin);

    loaded_library=LoadLibrary(path);

    if(!loaded_library)
        return 0;

    library=new(SystemLibrary);
    library->library=loaded_library;
    library->name=name;

    return library;
}


void close_system_library(SystemLibrary* library)
{
    FreeLibrary(library->library);
    str_free(library->name);
    free(library);
}


char* get_system_function(SystemLibrary* library, String *name)
{
    return GetProcAddress(library->library, name->begin);
}

//#endif // WIN32

int run_system_function(int(*f)(), int *args, int length)
{
    for(length--; length>=0; length--)
    {
        __asm__ volatile(
            "sub   $4,  %%esp  \n"
            "movl  %0, (%%esp)   "
            :
            :"r"(args[length])
            );
    }

    return f();
}

#include <stdio.h>
#include <stdlib.h>

#include "parser/parser.h"
#include <locale.h>


char debug=1;


int main()
{
    setlocale(LC_ALL, "rus");

    FILE     *f=fopen("test.txt", "rb");
    Function *main;

    init_parser();
    init_interpretator();

    main=run_parser(f, fgetc, feof);
/*
    if(main)
        interpretator(main);
    else
        printf("\nскомпилировалось с ошибкой");
*/
    return 0;
}

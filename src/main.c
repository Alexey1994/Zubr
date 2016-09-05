#include <stdio.h>
#include <stdlib.h>

#include "parser/parser.h"
#include "translator/translator.h"
#include <locale.h>


char debug=1;


int main()
{
    setlocale(LC_ALL, "rus");

    FILE     *f=fopen("test.txt", "rb");
    Function *main;

    init_parser();
    init_interpretator();

    //main=run_parser(f, fgetc, feof);
/*
    if(main)
        interpretator(main);
    else
        printf("\nскомпилировалось с ошибкой");
*/

    FILE *file_translator=fopen("translator.txt", "rb");

    init_translator_parser_states();
    parse_translator(file_translator, fgetc, feof);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>

#include "lexer/lexer.h"

#include <locale.h>

void read_byte(Lexer *lexer)
{
    if(lexer->buffered_symbol_pos<lexer->buffered_length)
    {
        lexer->head=lexer->buffer[lexer->buffered_symbol_pos];
        lexer->buffered_symbol_pos++;

        if(lexer->buffered_symbol_pos==lexer->buffered_length)
        {
            lexer->buffered_length=0;
            lexer->buffered_symbol_pos=0;
        }
    }
    else
    {
        lexer->head=fgetc(lexer->source);

        if(feof((FILE*)lexer->source))
            lexer->end_of_data=1;
    }

    //if(!lexer->end_of_data)
        //printf("%c", lexer->head);
}

int main()
{
    /*
    FILE *t=fopen("a.txt", "rb");
    char head=fgetc(t);

    if(is_true_word(t, &head, "fun"))
    {
        printf(":)");
    }

    fclose(t);
    return 0;*/

    setlocale(LC_ALL, "rus");

    FILE *f=fopen("test.txt", "rb");
    char *main;

    lexer_table_init();
    interpretator_table_init();
    interpretator_operation_table_init();

    main=run_lexer(f, read_byte);

    if(main)
    {
        //printf("\ninterpretator\n");
        interpretator(main);
    }

    return 0;
}

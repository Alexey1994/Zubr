#include "translator.h"
#include "types.h"
#include "translator commands/if.h"
#include "translator commands/print.h"
#include "translator commands/end.h"


static void (*translator_table[256])(Translator *translator, char *data);


void init_translator_table()
{
    int i;

    for(i=0; i<256; i++)
        translator_table[i]=0;

    translator_table[IF]=translate_if;
    translator_table[PRINT]=translate_print;
    translator_table[END]=translate_end;
}


void translate(Function *main, PrefixTree *translator_commands, char  *out_source, void (*out_byte)(char data, char *source))
{
    Data  *body_data;
    void (*translator_func)(Translator *translator, char *data);
    Translator translator;

    translator.out_byte     = out_byte;
    translator.out_source   = out_source;
    translator.commands     = translator_commands;
    translator.stack_blocks = array_init(4);
    translator.current_body = main->body;
    translator.current_i    = 0;

    for(;;)
    {
        for(; translator.current_i<translator.current_body->length; translator.current_i++)
        {
            body_data=translator.current_body->data[translator.current_i];

            translator_func=translator_table[body_data->type];

            if(translator_func)
                translator_func(&translator, body_data->data);
        }

        if(translator.stack_blocks->length)
        {
            translator.current_i=array_pop(translator.stack_blocks);
            translator.current_body=array_pop(translator.stack_blocks);

            //printf("\n%d ", translator.current_i);

            translator_table[END](&translator, 0);
            //out_token_translator_parser("\r\n}", &translator);
        }
        else
            break;
    }
}

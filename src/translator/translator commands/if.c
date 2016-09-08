#include "if.h"


void translate_if(Translator *translator, If *if_data)
{
    Array *body=find_data_in_prefix_tree(translator->commands, "if");

    write_translator_body(translator, body);

    array_push(translator->stack_blocks, translator->current_body);
    array_push(translator->stack_blocks, translator->current_i+1);

    translator->current_body=if_data->body;
    translator->current_i=-1;
}

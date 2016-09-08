#include "end.h"


void translate_end(Translator *translator)
{
    Array *body=find_data_in_prefix_tree(translator->commands, "end");

    write_translator_body(translator, body);
}

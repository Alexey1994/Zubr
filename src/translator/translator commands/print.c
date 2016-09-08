 #include "print.h"


void translate_print(Translator *translator, Array *expression)
{
    //out_token_translator_parser("console.log('hello')\r\n", translator);

    Array *body=find_data_in_prefix_tree(translator->commands, "print");

    translator->current_expression=expression;
    write_translator_body(translator, body);
}

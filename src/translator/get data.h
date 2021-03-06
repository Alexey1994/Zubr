#ifndef TRANSLATOR_GET_DATA_H_INCLUDED
#define TRANSLATOR_GET_DATA_H_INCLUDED

#include "translator.h"
#include "../string.h"


void skip_translator_parser(TranslatorParser *parser);
void get_byte_translator_parser(TranslatorParser *parser);
char* get_new_token_translator_parser(TranslatorParser *parser);
char* get_token_translator_parser(TranslatorParser *parser);

void out_token_translator_parser(char *token, Translator *translator);


#endif // TRANSLATOR_GET_DATA_H_INCLUDED

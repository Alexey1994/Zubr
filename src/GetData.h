#ifndef GETDATA_H_INCLUDED
#define GETDATA_H_INCLUDED

#include "parser/parser.h"


Data* new_data(char *data, char type);
String* get_new_token(Parser *parser);
char is_true_word(Parser *parser, char *word);
void read_byte(Parser *parser);

#endif // GETDATA_H_INCLUDED

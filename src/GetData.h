#ifndef GETDATA_H_INCLUDED
#define GETDATA_H_INCLUDED

#include "parser/parser.h"

char is_hex_number(char c);
char is_number(char c);

Data* new_data(char *data, char type);
String* get_new_token(Parser *parser);
char is_true_word(Parser *parser, char *word);
void read_byte(Parser *parser);


void get_token_data();

#endif // GETDATA_H_INCLUDED

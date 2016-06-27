#ifndef GETDATA_H_INCLUDED
#define GETDATA_H_INCLUDED

#include "lexer/lexer_operations.h"

char is_hex_number(char c);
char is_number(char c);

Data* new_data(char *data, char type);
String* get_token_data(Lexer *lexer);
char is_true_word(Lexer *lexer, char *word);
void read_byte(Lexer *lexer);

#endif // GETDATA_H_INCLUDED

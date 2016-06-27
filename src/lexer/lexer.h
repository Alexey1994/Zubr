#ifndef LEXER_H_INCLUDED
#define LEXER_H_INCLUDED

#include <stdio.h>
#include "../tree.h"
#include "lexer_operations.h"

Function* run_lexer(char *source, char (*get_byte)(Lexer *lexer), char (*end_of_data)(Lexer *lexer));
void lexer_init();

#endif // LEXER_H_INCLUDED

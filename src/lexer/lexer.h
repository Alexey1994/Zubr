#ifndef LEXER_H_INCLUDED
#define LEXER_H_INCLUDED

#include <stdio.h>
#include "../tree.h"
#include "lexer_operations.h"

Function* run_lexer(char *source, void (*read_byte(Lexer *lexer)));
void lexer_table_init();

#endif // LEXER_H_INCLUDED

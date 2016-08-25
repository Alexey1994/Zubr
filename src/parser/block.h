#ifndef BLOCK_H_INCLUDED
#define BLOCK_H_INCLUDED

#include "parser.h"

void add_block(Parser *parser, char *new_block, char block_type, Array *block_body);
void update_cur_body(Parser *parser);

#endif // BLOCK_H_INCLUDED

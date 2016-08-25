#include "debug.h"


char debug;
int  block_pos=0;

void print_block()
{
    int i;

    for(i=0; i<block_pos; i++)
        printf("  ");
}

#include "character.h"


char is_number(char c)
{
    if(c>='0' && c<='9')
        return 1;

    return 0;
}


char is_hex_number(char c)
{
    if((c>='0' && c<='9') || (c>='a' && c<='f') || (c>='A' && c<='F'))
        return 1;

    return 0;
}


char is_letter(char c)
{
    if((c>='a' && c<='z') || (c>='A' && c<='Z') || c=='_')
        return 1;

    return 0;
}


char is_space(char c)
{
    if(c==' ' || c=='\n' || c=='\r' || c=='\t')
        return 1;

    return 0;
}

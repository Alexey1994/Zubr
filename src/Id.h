#ifndef ID_H_INCLUDED
#define ID_H_INCLUDED

typedef struct
{
    char *prev_id;
    char min, max;
}Id;

Id* init_id(char min, char max);
char *generate_id(Id *id);


#endif // ID_H_INCLUDED

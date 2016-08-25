#ifndef ARRAY_H_INCLUDED
#define ARRAY_H_INCLUDED

typedef struct
{
    char **data;
    int    length;
    int    buffer_length;
    int    reserve;
}
Array;

Array* array_init(int reserve);
void array_push(Array *array, char *data);
char* array_pop(Array *array);
void array_clear(Array *array);
void array_free(Array *array);

#endif // ARRAY_H_INCLUDED

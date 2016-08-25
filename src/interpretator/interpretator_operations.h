#ifndef INTERPRETATOR_OPERATIONS_H_INCLUDED
#define INTERPRETATOR_OPERATIONS_H_INCLUDED

//#include "../parser/operations.h"
#include "../types.h"
#include "../Array.h"
#include "interpretator.h"

#define LOOP_NORMAL   0
#define LOOP_BREAK    1
#define LOOP_CONTINUE 2
//#define FUNCTION_RETURN 3

extern Variable *undefined;

void interpretator_table_init();

char interpretator_loop(Array *loop_body, Interpretator *interpretator);
char interpretator_while(While *while_data, Interpretator *intepretator);
char interpretator_do(Do *do_data, Interpretator *intepretator);
char interpretator_break();
char interpretator_continue();
char interpretator_print(Array *expression, Interpretator *interpretator);
char interpretator_if(If *if_data, Interpretator *interpretator);
char interpretator_call(Array *call_data, Interpretator *interpretator);
char interpretator_system_call(SystemCall *system_call_data, Interpretator *interpretator);
char interpretator_assignment(Assignment *assignment_data, Interpretator *interpretator);
char interpretator_push(Push *push_data, Interpretator *interpretator);

#endif // INTERPRETATOR_OPERATIONS_H_INCLUDED

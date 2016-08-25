#ifndef EVALEXPRESSION_H_INCLUDED
#define EVALEXPRESSION_H_INCLUDED

#include "interpretator.h"

void interpretator_operation_table_init();
Variable* eval_operand(Array *arr, Interpretator *interpretator);
Variable* eval(Interpretator *interpretator, Array *expression);

#endif // EVALEXPRESSION_H_INCLUDED

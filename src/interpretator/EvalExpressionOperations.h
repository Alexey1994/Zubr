#ifndef EVALEXPRESSIONOPERATIONS_H_INCLUDED
#define EVALEXPRESSIONOPERATIONS_H_INCLUDED

#include "interpretator.h"

void add(Interpretator *interpretator);
void sub(Interpretator *interpretator);
void mul(Interpretator *interpretator);
void division(Interpretator *interpretator);

void not_equal(Interpretator *interpretator);
void equal(Interpretator *interpretator);
void lesser(Interpretator *interpretator);
void lesser_or_equal(Interpretator *interpretator);
void greather(Interpretator *interpretator);
void greather_or_equal(Interpretator *interpretator);

void and(Interpretator *interpretator);

void unary_negate(Interpretator *interpretator);

#endif // EVALEXPRESSIONOPERATIONS_H_INCLUDED

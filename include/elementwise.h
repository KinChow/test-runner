#ifndef ELEMENTWISE_H
#define ELEMENTWISE_H

#include <cstddef>

void ElementwiseAdd(float *a, float *b, float *result, size_t size);

void ElementwiseAddUnroll(float *a, float *b, float *result, size_t size);

#endif  // ELEMENTWISE_H
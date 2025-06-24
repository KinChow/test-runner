#include "elementwise.h"

void ElementwiseAdd(float *a, float *b, float *result, size_t size)
{
    for (size_t i = 0; i < size; ++i) {
        result[i] = a[i] + b[i];
    }
}

void ElementwiseAddUnroll(float *a, float *b, float *result, size_t size)
{
    size_t i = 0;
    for (; i + 3 < size; i += 4) {
        result[i] = a[i] + b[i];
        result[i + 1] = a[i + 1] + b[i + 1];
        result[i + 2] = a[i + 2] + b[i + 2];
        result[i + 3] = a[i + 3] + b[i + 3];
    }
    for (; i < size; ++i) {
        result[i] = a[i] + b[i];
    }
}
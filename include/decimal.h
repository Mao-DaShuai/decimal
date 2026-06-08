#ifndef _DECIMAL_H_
#define _DECIMAL_H_

#include <stdint.h>


typedef struct decimal {
    char *value;
    int size; // number of digits
    int scale; // number of digits after the decimal point
    int sign; // 0 for positive, 1 for negative
}decimal_t;


decimal_t *decimal_alloc_int64(int64_t value);
decimal_t *decimal_alloc_double(double value);
decimal_t *decimal_alloc_string(const char *str);
decimal_t *decimal_static_string(decimal_t * dml, const char *str);
decimal_t *decimal_add(decimal_t *a, decimal_t *b);
decimal_t *decimal_sub(decimal_t *a, decimal_t *b);
decimal_t *decimal_mul(decimal_t *a, decimal_t *b);
decimal_t *decimal_div(decimal_t *a, decimal_t *b);
int decimal_to_double(decimal_t *d, double *value);
void decimal_free(decimal_t *d);
int decimal_compare(decimal_t *a, decimal_t *b);
char *decimal_to_string(char *buf, decimal_t *d);

#endif /* _DECIMAL_H_ */
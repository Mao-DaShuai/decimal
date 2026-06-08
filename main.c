#include <stdio.h>
#include "decimal.h"


int main() {
    char buffer[64];
    decimal_t *d = decimal_alloc_int64(-1234567890123456789LL);
    decimal_t *d2 = decimal_alloc_string("3.141592653545676999");

    // decimal_to_string(buffer, d);
    // printf("d = %s\n", buffer);
    // decimal_to_string(buffer, d2);
    // printf("d2 = %s\n", buffer);

    // decimal_add(d, d2);
    // decimal_to_string(buffer, d);
    // printf("d + d2 = %s\n", buffer);

    // decimal_sub(d, d2);
    // decimal_to_string(buffer, d);
    // printf("d - d2 = %s\n", buffer);

    decimal_mul(d, d2);
    decimal_to_string(buffer, d);
    printf("d * d2 = %s\n", buffer);

    double PI;
    int scale;
    decimal_t *d3 = decimal_alloc_double(3.1415926535);
    if (!(scale = decimal_to_double(d3, &PI))) {
        printf("获取失败!\n");
    }
    printf("PI = %.*f\n", scale, PI);
    printf("scale = %d\n", scale);

    decimal_free(d);
    decimal_free(d2);
    decimal_free(d3);

    return 0;
}
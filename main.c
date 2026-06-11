#include <stdio.h>
#include <string.h>
#include "decimal.h"

int main()
{
    char buf[256];
    int pass = 0, fail = 0;

#define TEST(cond, msg) do {                                    \
    if (!(cond)) {                                              \
        printf("  FAIL: %s\n", msg); fail++;                    \
    } else {                                                    \
        printf("  PASS: %s\n", msg); pass++;                    \
    }                                                           \
} while(0)

#define CHECK_VAL(label, d, expected) do {                      \
    decimal_to_string(buf, d);                                  \
    if (strcmp(buf, expected) != 0) {                           \
        printf("  FAIL: %s = %s (expected %s)\n",               \
               label, buf, expected); fail++;                   \
    } else {                                                    \
        printf("  PASS: %s = %s\n", label, expected); pass++;   \
    }                                                           \
} while(0)

    printf("====== alloc_int64 ======\n");
    {
        decimal_t *d = decimal_alloc_int64(0);
        CHECK_VAL("zero",      d, "0.0");
        decimal_free(d);
    }
    {
        decimal_t *d = decimal_alloc_int64(-1234567890123456789LL);
        CHECK_VAL("neg int64", d, "-1234567890123456789.0");
        decimal_free(d);
    }

    printf("\n====== alloc_double ======\n");
    {
        decimal_t *d = decimal_alloc_double(3.14159265358979);
        CHECK_VAL("pi", d, "3.14159265358979");
        decimal_free(d);
    }

    printf("\n====== alloc_string ======\n");
    {
        decimal_t *d = decimal_alloc_string("123.45");
        CHECK_VAL("positive", d, "123.45");
        decimal_free(d);
    }
    {
        decimal_t *d = decimal_alloc_string("-3.14");
        CHECK_VAL("negative", d, "-3.14");
        decimal_free(d);
    }
    {
        decimal_t *d = decimal_alloc_string("42");
        CHECK_VAL("no dot", d, "42.0");
        decimal_free(d);
    }
    {
        decimal_t *d = decimal_alloc_string("-7");
        CHECK_VAL("neg no dot", d, "-7.0");
        decimal_free(d);
    }

    printf("\n====== addition ======\n");
    {
        decimal_t *a = decimal_alloc_string("1.5");
        decimal_t *b = decimal_alloc_string("2.3");
        decimal_add(a, b);
        CHECK_VAL("1.5 + 2.3", a, "3.8");
        decimal_free(a); decimal_free(b);
    }
    {
        decimal_t *a = decimal_alloc_string("999.9");
        decimal_t *b = decimal_alloc_string("0.1");
        decimal_add(a, b);
        CHECK_VAL("999.9 + 0.1", a, "1000.0");
        decimal_free(a); decimal_free(b);
    }
    {
        decimal_t *a = decimal_alloc_string("123.456");
        decimal_t *b = decimal_alloc_string("0.001");
        decimal_add(a, b);
        CHECK_VAL("123.456 + 0.001", a, "123.457");
        decimal_free(a); decimal_free(b);
    }
    {
        decimal_t *a = decimal_alloc_string("5.5");
        decimal_t *b = decimal_alloc_string("-2.2");
        decimal_add(a, b);
        CHECK_VAL("5.5 + (-2.2)", a, "3.3");
        decimal_free(a); decimal_free(b);
    }
    {
        decimal_t *a = decimal_alloc_string("2.2");
        decimal_t *b = decimal_alloc_string("-5.5");
        decimal_add(a, b);
        CHECK_VAL("2.2 + (-5.5)", a, "-3.3");
        decimal_free(a); decimal_free(b);
    }

    printf("\n====== subtraction ======\n");
    {
        decimal_t *a = decimal_alloc_string("10.0");
        decimal_t *b = decimal_alloc_string("4.5");
        decimal_sub(a, b);
        CHECK_VAL("10.0 - 4.5", a, "5.5");
        decimal_free(a); decimal_free(b);
    }
    {
        decimal_t *a = decimal_alloc_string("4.5");
        decimal_t *b = decimal_alloc_string("10.0");
        decimal_sub(a, b);
        CHECK_VAL("4.5 - 10.0", a, "-5.5");
        decimal_free(a); decimal_free(b);
    }
    {
        decimal_t *a = decimal_alloc_string("-5.0");
        decimal_t *b = decimal_alloc_string("3.0");
        decimal_sub(a, b);
        CHECK_VAL("-5.0 - 3.0", a, "-8.0");
        decimal_free(a); decimal_free(b);
    }
    {
        decimal_t *a = decimal_alloc_string("-5.0");
        decimal_t *b = decimal_alloc_string("-3.0");
        decimal_sub(a, b);
        CHECK_VAL("-5.0 - (-3.0)", a, "-2.0");
        decimal_free(a); decimal_free(b);
    }

    printf("\n====== multiplication ======\n");
    {
        decimal_t *a = decimal_alloc_string("3.14");
        decimal_t *b = decimal_alloc_string("2.0");
        decimal_mul(a, b);
        CHECK_VAL("3.14 * 2.0", a, "6.28");
        decimal_free(a); decimal_free(b);
    }
    {
        decimal_t *a = decimal_alloc_string("-2.5");
        decimal_t *b = decimal_alloc_string("4.0");
        decimal_mul(a, b);
        CHECK_VAL("-2.5 * 4.0", a, "-10.0");
        decimal_free(a); decimal_free(b);
    }
    {
        decimal_t *a = decimal_alloc_string("0.1");
        decimal_t *b = decimal_alloc_string("0.1");
        decimal_mul(a, b);
        CHECK_VAL("0.1 * 0.1", a, "0.01");
        decimal_free(a); decimal_free(b);
    }
    {
        decimal_t *a = decimal_alloc_string("999.9");
        decimal_t *b = decimal_alloc_string("0.0");
        decimal_mul(a, b);
        CHECK_VAL("999.9 * 0.0", a, "0.0");
        decimal_free(a); decimal_free(b);
    }

    printf("\n====== division ======\n");
    {
        decimal_t *a = decimal_alloc_string("10.0");
        decimal_t *b = decimal_alloc_string("3.0");
        decimal_div(a, b);
        printf("  INFO: 10.0 / 3.0 = %s\n", decimal_to_string(buf, a));
        decimal_free(a); decimal_free(b);
    }
    {
        decimal_t *a = decimal_alloc_string("7.5");
        decimal_t *b = decimal_alloc_string("2.5");
        decimal_div(a, b);
        CHECK_VAL("7.5 / 2.5", a, "3.0");
        decimal_free(a); decimal_free(b);
    }
    {
        decimal_t *a = decimal_alloc_string("1.0");
        decimal_t *b = decimal_alloc_string("4.0");
        decimal_div(a, b);
        CHECK_VAL("1.0 / 4.0", a, "0.25");
        decimal_free(a); decimal_free(b);
    }
    {
        decimal_t *a = decimal_alloc_string("-9.0");
        decimal_t *b = decimal_alloc_string("3.0");
        decimal_div(a, b);
        CHECK_VAL("-9.0 / 3.0", a, "-3.0");
        decimal_free(a); decimal_free(b);
    }
    {
        decimal_t *a = decimal_alloc_string("9.0");
        decimal_t *b = decimal_alloc_string("-3.0");
        decimal_div(a, b);
        CHECK_VAL("9.0 / -3.0", a, "-3.0");
        decimal_free(a); decimal_free(b);
    }
    {
        decimal_t *a = decimal_alloc_string("-9.0");
        decimal_t *b = decimal_alloc_string("-3.0");
        decimal_div(a, b);
        CHECK_VAL("-9.0 / -3.0", a, "3.0");
        decimal_free(a); decimal_free(b);
    }
    {
        decimal_t *a = decimal_alloc_string("0.0");
        decimal_t *b = decimal_alloc_string("123.456");
        decimal_div(a, b);
        CHECK_VAL("0.0 / 123.456", a, "0.0");
        decimal_free(a); decimal_free(b);
    }
    {
        decimal_t *a = decimal_alloc_string("1.0");
        decimal_t *b = decimal_alloc_string("0.0");
        decimal_t *ret = decimal_div(a, b);
        TEST(ret == NULL, "1.0 / 0.0 returns NULL");
        decimal_free(a); decimal_free(b);
    }
    {
        decimal_t *a = decimal_alloc_string("123.45");
        decimal_t *b = decimal_alloc_string("6.7");
        decimal_div(a, b);
        printf("  INFO: 123.45 / 6.7 = %s\n", decimal_to_string(buf, a));
        decimal_free(a); decimal_free(b);
    }

    printf("\n====== comparison ======\n");
    {
        decimal_t *a = decimal_alloc_string("5.0");
        decimal_t *b = decimal_alloc_string("3.0");
        TEST(decimal_compare(a, b) == 1, "5.0 > 3.0");
        TEST(decimal_compare(b, a) == -1, "3.0 < 5.0");
        decimal_free(a); decimal_free(b);
    }
    {
        decimal_t *a = decimal_alloc_string("3.14");
        decimal_t *b = decimal_alloc_string("3.14");
        TEST(decimal_compare(a, b) == 0, "3.14 == 3.14");
        decimal_free(a); decimal_free(b);
    }
    {
        decimal_t *a = decimal_alloc_string("-5.0");
        decimal_t *b = decimal_alloc_string("3.0");
        TEST(decimal_compare(a, b) == -1, "-5.0 < 3.0");
        TEST(decimal_compare(b, a) == 1, "3.0 > -5.0");
        decimal_free(a); decimal_free(b);
    }
    {
        decimal_t *a = decimal_alloc_string("0.01");
        decimal_t *b = decimal_alloc_string("0.009");
        TEST(decimal_compare(a, b) == 1, "0.01 > 0.009");
        decimal_free(a); decimal_free(b);
    }

    printf("\n====== to_double ======\n");
    {
        decimal_t *d = decimal_alloc_double(3.14159265358979);
        double val;
        int s = decimal_to_double(d, &val);
        printf("  INFO: double PI = %.*f (scale=%d)\n", s, val, s);
        decimal_free(d);
    }

    printf("\n====== copy ======\n");
    {
        decimal_t *a = decimal_alloc_string("42.5");
        decimal_t *b = decimal_alloc_int64(0);
        decimal_copy(b, a);
        CHECK_VAL("copy a->b", b, "42.5");
        decimal_free(a); decimal_free(b);
    }

    printf("\n====== mixed operations ======\n");
    {
        decimal_t *a = decimal_alloc_string("10.0");
        decimal_t *b = decimal_alloc_string("2.0");
        decimal_add(a, b);
        decimal_t *c = decimal_alloc_string("3.0");
        decimal_mul(a, c);
        decimal_t *d = decimal_alloc_string("4.0");
        decimal_t *e = decimal_alloc_string("2.0");
        decimal_div(d, e);
        decimal_sub(a, d);
        CHECK_VAL("(10+2)*3 - 4/2", a, "34.0");
        decimal_free(a); decimal_free(b); decimal_free(c);
        decimal_free(d); decimal_free(e);
    }

    printf("\n==============================\n");
    printf("Results: %d passed, %d failed\n", pass, fail);
    return fail ? 1 : 0;
}
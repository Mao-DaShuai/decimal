# Decimal - High-Precision Decimal Arithmetic Library for C

> [中文版](README.zh-CN.md)

A C library for arbitrary-precision decimal arithmetic, supporting addition, subtraction, multiplication, and conversion operations with exact decimal representation.

## Features

- **Arbitrary precision** - Not limited by native floating-point precision
- **Exact decimal representation** - Avoids binary floating-point rounding errors
- **Multiple constructors** - Create decimals from `int64_t`, `double`, or string
- **Basic arithmetic** - Addition, subtraction, multiplication
- **Comparison** - Compare two decimal values
- **Conversion** - Convert to `double` or string representation
- **Memory efficient** - Static and dynamic allocation options

## Data Structure

```c
typedef struct decimal {
    char *value;   // String representation of the number
    int size;      // Number of digits
    int scale;     // Number of digits after the decimal point
    int sign;      // 0 for positive, 1 for negative
} decimal_t;
```

## API Overview

| Function | Description |
|----------|-------------|
| `decimal_alloc_int64` | Create a decimal from `int64_t` |
| `decimal_alloc_double` | Create a decimal from `double` |
| `decimal_alloc_string` | Create a decimal from string |
| `decimal_static_string` | Initialize a pre-allocated decimal from string |
| `decimal_add` | Add two decimals |
| `decimal_sub` | Subtract two decimals |
| `decimal_mul` | Multiply two decimals |
| `decimal_div` | Divide two decimals (in progress) |
| `decimal_compare` | Compare two decimals |
| `decimal_to_double` | Convert to `double` |
| `decimal_to_string` | Convert to string |
| `decimal_free` | Free allocated memory |

## Example

```c
#include <stdio.h>
#include "decimal.h"

int main() {
    char buffer[64];

    decimal_t *a = decimal_alloc_string("123.456");
    decimal_t *b = decimal_alloc_string("78.910");

    decimal_add(a, b);
    decimal_to_string(buffer, a);
    printf("a + b = %s\n", buffer);  // "202.366"

    decimal_free(a);
    decimal_free(b);
    return 0;
}
```

## Build

Compile with any C compiler (C99 or later):

```bash
gcc -o program main.c src/decimal.c -Iinclude
```
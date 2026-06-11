# Decimal - High-Precision Decimal Arithmetic Library for C

> [中文版](README.zh-CN.md)

A C library for arbitrary-precision decimal arithmetic, supporting addition, subtraction, multiplication, division, and conversion operations with exact decimal representation.

## Features

- **Arbitrary precision** - Not limited by native floating-point precision
- **Exact decimal representation** - Avoids binary floating-point rounding errors
- **Multiple constructors** - Create decimals from `int64_t`, `double`, or string
- **Full arithmetic** - Addition, subtraction, multiplication, and division
- **High-precision division** - Long division algorithm with configurable precision (default 50 decimal places)
- **Comparison** - Compare two decimal values
- **Conversion** - Convert to `double` or string representation
- **Memory efficient** - Static and dynamic allocation options

## Data Structure

```c
typedef struct decimal {
    char *value;   /**< String representation of the number (includes decimal point) */
    int size;      /**< Total string length */
    int scale;     /**< Number of digits after the decimal point */
    int sign;      /**< 0 for positive, 1 for negative */
} decimal_t;
```

## API Overview

| Function | Description |
|----------|-------------|
| `decimal_alloc_int64` | Create a decimal from `int64_t` |
| `decimal_alloc_double` | Create a decimal from `double` |
| `decimal_alloc_string` | Create a decimal from string |
| `decimal_static_string` | Initialize a pre-allocated decimal from string |
| `decimal_copy` | Copy one decimal to another |
| `decimal_add` | Add two decimals (result stored in first operand) |
| `decimal_sub` | Subtract two decimals (result stored in first operand) |
| `decimal_mul` | Multiply two decimals (result stored in first operand) |
| `decimal_div` | Divide two decimals (result stored in first operand) |
| `decimal_compare` | Compare two decimals (returns -1, 0, or 1) |
| `decimal_to_double` | Convert to `double` |
| `decimal_to_string` | Convert to string |
| `decimal_free` | Free allocated memory |

## Example

```c
#include <stdio.h>
#include "decimal.h"

int main() {
    char buffer[128];

    decimal_t *a = decimal_alloc_string("10.0");
    decimal_t *b = decimal_alloc_string("3.0");

    decimal_div(a, b);
    decimal_to_string(buffer, a);
    printf("10 / 3 = %s\n", buffer);  // "3.33333... (50 decimal places)"

    decimal_t *c = decimal_alloc_string("123.45");
    decimal_t *d = decimal_alloc_string("6.7");

    decimal_div(c, d);
    decimal_to_string(buffer, c);
    printf("123.45 / 6.7 = %s\n", buffer);  // "184.25373..."

    decimal_free(a);
    decimal_free(b);
    decimal_free(c);
    decimal_free(d);
    return 0;
}
```

## Build

Compile with any C compiler (C99 or later):

```bash
gcc -o program main.c src/decimal.c -Iinclude -lm
```

## Project Structure

```
decimal/
├── include/
│   └── decimal.h      # Public API header with Doxygen-style documentation
├── src/
│   └── decimal.c      # Implementation
├── main.c              # Test program
├── README.md           # English documentation
└── README.zh-CN.md     # Chinese documentation
```
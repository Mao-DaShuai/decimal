# Decimal - 高精度十进制运算库

> [English Version](README.md)

一个 C 语言高精度十进制运算库，支持加减乘除四则运算和类型转换，提供精确的十进制表示，避免二进制浮点数精度丢失问题。

## 功能特性

- **任意精度** - 不受原生浮点数精度限制
- **精确十进制** - 避免二进制浮点数舍入误差
- **多种构造方式** - 支持从 `int64_t`、`double`、字符串创建十进制数
- **完整运算** - 加、减、乘、除
- **高精度除法** - 采用长除法算法，默认精度可达 50 位小数
- **比较** - 比较两个十进制数的大小
- **转换** - 转换为 `double` 或字符串表示
- **灵活内存管理** - 支持动态分配和静态初始化

## 数据结构

```c
typedef struct decimal {
    char *value;   /**< 数字的字符串表示（包含小数点） */
    int size;      /**< 字符串总长度 */
    int scale;     /**< 小数点后的位数 */
    int sign;      /**< 0 为正数，1 为负数 */
} decimal_t;
```

## API 一览

| 函数 | 说明 |
|----------|------|
| `decimal_alloc_int64` | 从 `int64_t` 创建十进制数 |
| `decimal_alloc_double` | 从 `double` 创建十进制数 |
| `decimal_alloc_string` | 从字符串创建十进制数 |
| `decimal_static_string` | 用字符串初始化预分配的十进制结构体 |
| `decimal_copy` | 复制十进制数 |
| `decimal_add` | 两个十进制数相加（结果存入第一个操作数） |
| `decimal_sub` | 两个十进制数相减（结果存入第一个操作数） |
| `decimal_mul` | 两个十进制数相乘（结果存入第一个操作数） |
| `decimal_div` | 两个十进制数相除（结果存入第一个操作数） |
| `decimal_compare` | 比较两个十进制数（返回 -1, 0, 或 1） |
| `decimal_to_double` | 转换为 `double` |
| `decimal_to_string` | 转换为字符串 |
| `decimal_free` | 释放内存 |

## 示例

```c
#include <stdio.h>
#include "decimal.h"

int main() {
    char buffer[128];

    decimal_t *a = decimal_alloc_string("10.0");
    decimal_t *b = decimal_alloc_string("3.0");

    decimal_div(a, b);
    decimal_to_string(buffer, a);
    printf("10 / 3 = %s\n", buffer);  // "3.33333...（50 位小数）"

    decimal_t *c = decimal_alloc_string("123.45");
    decimal_t *d = decimal_alloc_string("6.7");

    decimal_div(c, d);
    decimal_to_string(buffer, c);
    printf("123.45 / 6.7 = %s\n", buffer);  // "18.42537..."

    decimal_free(a);
    decimal_free(b);
    decimal_free(c);
    decimal_free(d);
    return 0;
}
```

## 构建

使用任意 C 编译器（C99 或更新版本）编译：

```bash
gcc -o program main.c src/decimal.c -Iinclude -lm
```

## 项目结构

```
decimal/
├── include/
│   └── decimal.h      # 公开 API 头文件（含 Doxygen 风格注释）
├── src/
│   └── decimal.c      # 实现代码
├── main.c              # 测试程序
├── README.md           # 英文文档
└── README.zh-CN.md     # 中文文档
```
#ifndef _DECIMAL_H_
#define _DECIMAL_H_

#include <stdint.h>


typedef struct decimal {
    char *value;   /**< 数值的字符串表示（包含小数点） */
    int size;      /**< 字符串总长度 */
    int scale;     /**< 小数点后的位数 */
    int sign;      /**< 符号：0 为正数，1 为负数 */
} decimal_t;


/**
 * @brief 使用 64 位有符号整数创建新的十进制数。
 * @param value 64 位有符号整数值。
 * @return 成功返回新分配的 decimal_t 实例，失败返回 NULL。
 */
decimal_t *decimal_alloc_int64(int64_t value);

/**
 * @brief 使用双精度浮点数创建新的十进制数。
 * @param value 双精度浮点数值。
 * @return 成功返回新分配的 decimal_t 实例，失败返回 NULL。
 */
decimal_t *decimal_alloc_double(double value);

/**
 * @brief 使用字符串创建新的十进制数。
 * @param str 表示十进制数的字符串（如 "123.45"），可包含前导负号。
 * @return 成功返回新分配的 decimal_t 实例，失败返回 NULL。
 */
decimal_t *decimal_alloc_string(const char *str);

/**
 * @brief 将 src 的内容复制到 dst 中。
 * @param dst 目标十进制数（已有的 value 会被释放）。
 * @param src 源十进制数。
 * @return 返回 dst 指针。
 */
decimal_t *decimal_copy(decimal_t *dst, decimal_t *src);

/**
 * @brief 使用静态缓冲区初始化十进制数（不分配堆内存）。
 * @param dml 指向已分配 value 缓冲区的 decimal_t 实例。
 * @param str 表示十进制数的字符串。
 * @return 成功返回 dml 指针，失败返回 NULL。
 */
decimal_t *decimal_static_string(decimal_t *dml, const char *str);

/**
 * @brief 将 b 加到 a 上，结果存储在 a 中。
 * @param a 被加数（会被修改为结果）。
 * @param b 加数。
 * @return 返回 a 指针。
 */
decimal_t *decimal_add(decimal_t *a, decimal_t *b);

/**
 * @brief 从 a 中减去 b，结果存储在 a 中。
 * @param a 被减数（会被修改为结果）。
 * @param b 减数。
 * @return 返回 a 指针。
 */
decimal_t *decimal_sub(decimal_t *a, decimal_t *b);

/**
 * @brief 将 a 乘以 b，结果存储在 a 中。
 * @param a 被乘数（会被修改为结果）。
 * @param b 乘数。
 * @return 返回 a 指针。
 */
decimal_t *decimal_mul(decimal_t *a, decimal_t *b);

/**
 * @brief 将 a 除以 b，结果存储在 a 中。
 * @param a 被除数（会被修改为结果）。
 * @param b 除数（不能为 0）。
 * @return 成功返回 a 指针，除零错误返回 NULL。
 */
decimal_t *decimal_div(decimal_t *a, decimal_t *b);

/**
 * @brief 将十进制数转换为双精度浮点数。
 * @param d 十进制数。
 * @param value 输出参数，用于存储转换后的 double 值。
 * @return 返回十进制数的小数位数（scale）。
 */
int decimal_to_double(decimal_t *d, double *value);

/**
 * @brief 释放十进制数占用的内存。
 * @param d 需要释放的 decimal_t 实例。
 */
void decimal_free(decimal_t *d);

/**
 * @brief 比较两个十进制数的大小。
 * @param a 第一个十进制数。
 * @param b 第二个十进制数。
 * @return 如果 a > b 返回 1，a == b 返回 0，a < b 返回 -1。
 */
int decimal_compare(decimal_t *a, decimal_t *b);

/**
 * @brief 将十进制数格式化为字符串。
 * @param buf 输出缓冲区（必须足够大）。
 * @param d 十进制数。
 * @return 返回 buf 指针。
 */
char *decimal_to_string(char *buf, decimal_t *d);

#endif /* _DECIMAL_H_ */
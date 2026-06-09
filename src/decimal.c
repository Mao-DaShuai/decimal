#include "decimal.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>


/**
 * @brief 使用给定的值和刻度创建新的十进制数。
 * @param value 小数的整数值（不带小数点）。
 * @return 一个表示该十进制值的新decimal_t实例。
 */
decimal_t *decimal_alloc_int64(int64_t value)
{
    decimal_t *d = (decimal_t *)malloc(sizeof(decimal_t));
    if (!d) {
        return NULL;
    }

    // 将整数值转换为字符串表示形式
    char buffer[32]; // 用于保存整数字符串表示的缓冲区
    snprintf(buffer, sizeof(buffer), "%lld", value < 0 ? -value : value); // 使用绝对值来处理负数

    int size = strlen(buffer);
    
    // 为字符串分配内存
    d->value = (char *)malloc(size + 1); // 为空终止符加1
    if (!d->value) {
        free(d); // 如果内存分配失败，则释放decimal结构体
        return NULL;
    }
    memset(d->value, 0, size + 1); // 初始化分配的内存
    
    // 将整数值以字符串形式复制到结构中
    strcpy(d->value, buffer);
    strcat(d->value, ".0"); // 添加小数点和一个零以表示整数部分
    
    d->size = size + 2; // 更新大小以反映添加的小数点和零
    d->scale = 1; // 小数点后有一个数字
    d->sign = (value < 0) ? 1 : 0; // 根据值设置符号

    return d;

}


/**
 * @brief 使用给定的double值创建新的十进制数。
 * @param value 要转换为十进制的double值。
 * @return 表示该十进制值的新decimal_t实例。
 */
decimal_t *decimal_alloc_double(double value)
{
    decimal_t *d = (decimal_t *)malloc(sizeof(decimal_t));
    if (!d) {
        return NULL; // Memory allocation failed
    }

    //  将双精度值转换为字符串表示形式
    char buffer[64]; // 用于保存双精度浮点数字符串表示的缓冲区
    int precision = 0; // 设置转换的精度（小数点后位数）
    double tmp = value;
    while (precision <= 15 && fabs(tmp - round(tmp)) >= 1e-15) {
        precision++;
        tmp *= 10; // 将值放大以增加小数点后的位数
    }
    snprintf(buffer, sizeof(buffer), "%.*f", precision, value); // 使用指定的精度将双精度值转换为字符串

    // 计算该数值的大小（数字的位数+小数点）
    int size = strlen(buffer);
    
    // 为十进制值字符串分配内存
    d->value = (char *)malloc(size + 1); // 为空终止符加1
    if (!d->value) {
        free(d); // 如果内存分配失败，则释放decimal结构体
        return NULL;
    }
    memset(d->value, 0, size + 1); // 初始化分配的内存
    
    // 将双精度值以字符串形式复制到结构中
    strcpy(d->value, buffer);
    
    d->size = size;
    
    // 计算刻度（小数点后数字的个数）
    char *decimal_point = strchr(buffer, '.');
    if (!decimal_point) {
        strcat(d->value, ".0"); // 如果没有小数点，则添加一个小数点和一个零
        d->size += 2; // 更新大小以反映添加的小数点和零
    }
    d->scale = decimal_point ? strlen(decimal_point + 1) : 1; // 如果有小数点，则计算刻度
    
    d->sign = (value < 0) ? 1 : 0; // 根据值设置符号

    return d;
}


/**
 * @brief 使用给定的字符串表示创建新的十进制数。
 * @param str 十进制值的字符串表示。
 * @return 表示该十进制值的新decimal_t实例。
 */
decimal_t *decimal_alloc_string(const char *str)
{
    decimal_t *d = (decimal_t *)malloc(sizeof(decimal_t));
    if (!d) {
        return NULL; // 内存分配失败
    }

    // 计算十进制值的大小（数字的位数）
    int size = strlen(str);
    
    // 为十进制值字符串分配内存
    d->value = (char *)malloc(size + 1); // 为空终止符加1
    if (!d->value) {
        free(d); // 如果内存分配失败，则释放decimal结构体
        return NULL;
    }
    memset(d->value, 0, size + 1); // 初始化分配的内存
    
    // 将输入字符串复制到十进制结构中，如果字符串以负号开头，则跳过负号
    strcpy(d->value, str[0] == '-' ? str + 1 : str);
    
    d->size = size;
    
    // 计算刻度（小数点后数字的个数）
    char *decimal_point = strchr(str, '.');
    if (!decimal_point) {
        strcat(d->value, ".0"); // 如果没有小数点，则添加一个小数点和一个零
        d->size += 2; // 更新大小以反映添加的小数点和零
    }
    d->scale = decimal_point ? strlen(decimal_point + 1) : 1; // 如果有小数点，则计算刻度
    
    d->sign = (str[0] == '-') ? 1 : 0; // 根据第一个字符设置符号

    return d;
}


/**
 * @brief 使用给定的 结构体 和 字符串 静态创建新的十进制数。
 * @param dml 提前申请内存的结构体。
 * @param str 十进制值的字符串表示。
 * @return 表示该十进制值的decimal_t实例dml。
 */
decimal_t *decimal_static_string(decimal_t * dml, const char *str)
{
    if (!dml) {
        return NULL; // 内存分配失败
    }

    // 计算十进制值的大小（数字的位数）
    int size = strlen(str);
    
    if (!dml->value) {
        fprintf(stderr, "静态分配decimal_t.value 失败!\n");
        return NULL;
    }
    memset(dml->value, 0, size + 1); // 初始化分配的内存
    
    // 将输入字符串复制到十进制结构中，如果字符串以负号开头，则跳过负号
    strcpy(dml->value, str[0] == '-' ? str + 1 : str);
    
    dml->size = size;
    
    // 计算刻度（小数点后数字的个数）
    char *decimal_point = strchr(str, '.');
    if (!decimal_point) {
        strcat(dml->value, ".0"); // 如果没有小数点，则添加一个小数点和一个零
        dml->size += 2; // 更新大小以反映添加的小数点和零
    }
    dml->scale = decimal_point ? strlen(decimal_point + 1) : 1; // 如果有小数点，则计算刻度
    
    dml->sign = (str[0] == '-') ? 1 : 0; // 根据第一个字符设置符号

    return dml;
}


/**
 * @brief 将两个十进制数相加并返回结果。
 * @param a 第一个十进制数。
 * @param b 第二个十进制数。
 * @return 表示a和b之和的新decimal_t实例。
 */
decimal_t *decimal_add(decimal_t *a, decimal_t *b)
{
    if (!a || !b) {
        return NULL; // Invalid input
    }

    int max_integer = 0; // 用于存储整数部分最大位数的变量
    int max_scale   = 0; // 用于存储小数部分最大位数的变量
    uint8_t flag = 0; // 用于标记是否需要处理符号的变量
    if (a->scale > b->scale) {
        flag |= 0b001; // 标记a的小数部分较长
        max_scale = a->scale;
    } else if (a->scale == b->scale) {
        flag |= 0b101; // 标记a和b的小数部分长度相同
        max_scale = a->scale;
    } else {
        flag |= 0b000; // 标记b的小数部分较长
        max_scale = b->scale;
    }
    if ((a->size - a->scale) > (b->size - b->scale)) {
        flag |= 0b010; // 标记a的整数部分较长
        max_integer = a->size - a->scale - 1;
    } else if ((a->size - a->scale) == (b->size - b->scale)) {
        flag |= 0b1010; // 标记a的整数部分较长
        max_integer = a->size - a->scale - 1;
    } else {
        flag |= 0b000; // 标记b的整数部分较长
        max_integer = b->size - b->scale - 1;
    }

    char *decimal_a = (char *)malloc(max_integer + max_scale + 5); // 用于存储a并对齐小数点的字符串
    if (!decimal_a) {
        return NULL; // 内存分配失败
    }
    memset(decimal_a, 0, max_integer + max_scale + 5); // 初始化分配的内存
    char *result = decimal_a + max_integer + max_scale + 1; // 最大的整数部分 + 最大的小数部分 + 小数点
    result[0] = '\0'; // 初始化结果字符串为空
    result--; // 将结果指针移动到结果字符串的末尾，以便从后向前构建结果
    char *tmp_a = a->value + a->size - 1; // 指向a的最后一个字符的指针
    char *tmp_b = b->value + b->size - 1; // 指向b的最后一个字符的指针

    if (flag & 1) { // 如果a的小数部分较长
        if (a->sign == b->sign) { // 如果a和b的符号相同，则结果的符号与a和b相同
            while ((max_scale - b->scale) > 0) {
                *result = *tmp_a; // 将a的小数部分复制到结果中
                result--;
                tmp_a--;
                max_scale--;
            }
        } else {
            while ((max_scale - b->scale) > 0) {
                *result += ('9'+1) - *tmp_a; // 将a的小数部分复制到结果中
                if (*result < '0') { // 如果当前位的和小于0，则需要借位
                    *result = '0'; // 将当前位的结果加上10以得到正确的个位数字
                    *(result-1) = '0' - 1; // 将借位1存储在结果中
                }
                result--;
                tmp_a--;
                max_scale--;
            }
        }
    } else { // 如果b的小数部分较长
        if (a->sign == b->sign) { // 如果a和b的符号相同，则结果的符号与a和b相同
            while ((max_scale - a->scale) > 0) {
                *result = *tmp_b; // 将b的小数部分复制到结果中
                result--;
                tmp_b--;
                max_scale--;
            }
        } else {
            while ((max_scale - a->scale) > 0) {
                if (*tmp_b > '0') {
                    *result +=  *result ? ('9'+1) - *tmp_b : ('9'+1) - *tmp_b + '0'; // 将b的小数部分复制到结果中
                    *(result-1) = '0' - 1; // 将借位1存储在结果中
                } else {
                    *result += '0'; // 将b的小数部分复制到结果中
                }
                if (*result < '0') { // 如果当前位的和小于0，则需要借位
                    *result = '0'; // 将当前位的结果加上10以得到正确的个位数字
                    *(result-1) = '0' - 1; // 将借位1存储在结果中
                }
                result--;
                tmp_b--;
                max_scale--;
            }
        }
    }


    /**
     *     xxxx.xxxxxx
     *       xx.xxxxxxxxxxx
     *      结果字符串的构建是从后向前进行的，因此在处理小数部分时，首先将较长的小数部分复制到结果中，以确保小数点对齐。
     * 然后，在处理整数部分时，根据a和b的符号进行加法或减法操作，并将结果存储在结果字符串中。如果需要进位或借位，还会相应地调整结果字符串。
     * 最后，将构建好的结果字符串赋值给a，并更新a的size、scale和sign字段以反映新的十进制值。
     */
    while (tmp_a >= a->value && tmp_b >= b->value) {
        if (*tmp_a == '.') { // 如果当前字符是小数点，则将小数点复制到结果中
            if (*result) { // 如果结果字符串已经有内容，则需要在小数点前添加一个零
                *(result-1) = *result; // 将结果字符串中的内容向左移动一位
            }
            *result = '.';
            result--;
            tmp_a--;
            tmp_b--;
            continue;
        }

        int digit_a = (*tmp_a >= '0' && *tmp_a <= '9') ? (*tmp_a - '0') : 0; // 获取a当前位的数字
        int digit_b = (*tmp_b >= '0' && *tmp_b <= '9') ? (*tmp_b - '0') : 0; // 获取b当前位的数字
        if (a->sign != b->sign) { // 如果a和b的符号不同，则需要进行减法操作
            if (digit_a < digit_b) { // 如果a当前位的数字小于b当前位的数字，则需要借位
                digit_a += 10; // 将a当前位的数字加上10以进行借位
                *(result-1) = '0' - 1; // 将借位1存储在结果中
            }
            int diff = digit_a - digit_b; // 计算当前位的差值
            *result += *result ? diff :  diff + '0'; // 将当前位的结果转换为字符并存储在结果中
            if (*result < '0') { // 如果当前位的差值小于0，则需要借位
                *result = '0'; // 将当前位的结果加上10以得到正确的个位数字
                *(result-1) = '0' - 1; // 将借位1存储在结果中
            }
        } else { // 如果a和b的符号相同，则需要进行加法操作
            int sum = digit_a + digit_b; // 计算当前位的和
            *result += *result ? sum :  sum + '0'; // 将当前位的结果转换为字符并存储在结果中
            if (*result > '9') { // 如果当前位的和大于等于10，则需要进位
                *result -= 10; // 将当前位的结果减去10以得到正确的个位数字
                if ((result-1) < decimal_a) { // 如果结果指针已经超出分配的内存范围，则需要整体后移一位以腾出空间存储进位
                    for (int i=max_integer + max_scale + 3; i >= 0; i--) {
                        decimal_a[i+1] = decimal_a[i]; // 将现有的结果向右移动一位
                    }
                }
                *(result-1) = '1'; // 将进位1存储在结果中
            }
        }
        result--;
        tmp_a--;
        tmp_b--;
    }
    if (flag & 2) { // 如果a的整数部分较长
        while (tmp_a >= a->value) {
            *result += *result ? *tmp_a - '0' : *tmp_a; // 将当前位的结果转换为字符并存储在结果中
            if (*result > '9') { // 如果当前位的和大于等于10，则需要进位
                *result -= 10; // 将当前位的结果减去10以得到正确的个位数字
                if ((result-1) < decimal_a) { // 如果结果指针已经超出分配的内存范围，则需要重新分配内存
                    for (int i=max_integer + max_scale + 3; i >= 0; i--) {
                        decimal_a[i+1] = decimal_a[i]; // 将现有的结果向右移动一位
                    }
                }
                *(result-1) = '1'; // 将进位1存储在结果中
            } else if (*result < '0') { // 如果当前位的和小于0，则需要借位
                *result = '0'; // 将当前位的结果加上10以得到正确的个位数字
                *(result-1) = '0' - 1; // 将借位1存储在结果中
            }
            result--;
            tmp_a--;
        }
    } else { // 如果b的整数部分较长
        while (tmp_b >= b->value) {
            *result += *result ? *tmp_b - '0' : *tmp_b; // 将当前位的结果转换为字符并存储在结果中
            if (*result > '9') { // 如果当前位的和大于等于10，则需要进位
                *result -= 10; // 将当前位的结果减去10以得到正确的个位数字
                if ((result-1) < decimal_a) { // 如果结果指针已经超出分配的内存范围，则需要重新分配内存
                    for (int i=max_integer + max_scale + 3; i >= 0; i--) {
                        decimal_a[i+1] = decimal_a[i]; // 将现有的结果向右移动一位
                    }
                }
                *(result-1) = '1'; // 将进位1存储在结果中
            } else if (*result < '0') { // 如果当前位的和小于0，则需要借位
                *result = '0'; // 将当前位的结果加上10以得到正确的个位数字
                *(result-1) = '0' - 1; // 将借位1存储在结果中
            }
            result--;
            tmp_b--;
        }
    }

    // 去除首部的0字符
    result++; // 将结果指针移动回第一个数字的位置
    while (*result == '0' && *(result+1) != '.' && *(result+1) != '\0') {
        result++; // 将结果指针向右移动以跳过前导零
    }

    char *decimal_end = strchr(result, '.'); // 查找结果字符串中的小数点位置
    decimal_end += 1; // 将指针移动到小数点后第一个数字的位置
    char *tmp_end = decimal_a + max_integer + max_scale; // 结果字符串的末尾位置
    while (tmp_end > decimal_end && *tmp_end == '0') {
        *tmp_end = '\0'; // 将末尾的零字符替换为字符串终止符
        tmp_end--; // 将末尾指针向左移动以继续检查下一个字符
    }


    free(a->value); // 释放a原有的十进制值字符串的内存
    a->value = strdup(result); // 为新的十进制值字符串分配内存
    if (!a->value) {
        free(decimal_a); // 如果内存分配失败，则释放临时的结果字符串
        return NULL;
    }
    free(decimal_a); // 释放临时的结果字符串的内存

    a->size = strlen(a->value); // 更新a的size字段以反映结果的大小
    
    // 计算刻度（小数点后数字的个数）
    char *decimal_point = strchr(a->value, '.');
    a->scale = decimal_point ? strlen(decimal_point + 1) : 1; // 如果有小数点，则计算刻度

    if (a->sign != b->sign) { // 如果a和b的符号相同，则结果的符号与a和b相同
        if ((flag & 0b1111) == 0b1010) {
            a->sign = b->sign; // 如果a和b的符号不同，并且b的小数部分较长或a和b的整数部分长度相同，则结果为负数
        } else if ((flag & 0b1111) == 0b0000) {
            a->sign = b->sign;
        } else if ((flag & 0b11) == 0b01) {
            a->sign = b->sign; // 如果a和b的符号不同，并且b的整数部分较长，则结果为负数
        }
    }
    

    return NULL; // 这里应该继续实现十进制加法的逻辑，但目前返回NULL作为占位符
}


/**
 * @brief 将两个十进制数相减并返回结果。
 * @param a 第一个十进制数。
 * @param b 第二个十进制数。
 * @return 表示a和b之差的新decimal_t实例。
 */
decimal_t *decimal_sub(decimal_t *a, decimal_t *b)
{
    if (!a || !b) {
        return NULL; // Invalid input
    }

    // 通过将b的符号取反并调用decimal_add函数来实现十进制数的减法
    decimal_t *neg_b = (decimal_t *)malloc(sizeof(decimal_t)); // 创建一个新的decimal_t实例来存储b的负数表示
    if (!neg_b) {
        return NULL; // 内存分配失败
    }
    neg_b->value = strdup(b->value); // 复制b的十进制值字符串到neg_b中
    if (!neg_b->value) {
        free(neg_b); // 如果内存分配失败，则释放neg_b结构体
        return NULL;
    }
    neg_b->size = b->size; // 设置neg_b的大小与b相同
    neg_b->scale = b->scale; // 设置neg_b的刻度与b相同
    neg_b->sign = !b->sign; // 将b的符号取反以得到neg_b的符号

    decimal_t *result = decimal_add(a, neg_b); // 调用decimal_add函数将a和neg_b相加以实现a和b的减法

    decimal_free(neg_b); // 释放neg_b占用的内存

    return result; // 返回计算结果
}


/**
 * @brief 将两个十进制数相乘并返回结果。
 * @param a 第一个十进制数。
 * @param b 第二个十进制数。
 * @return 表示a和b之积的新decimal_t实例。
 */
decimal_t *decimal_mul(decimal_t *a, decimal_t *b)
{
    decimal_t *ret_dml = NULL;
    decimal_t *result = NULL;
    decimal_t *tmp_decimal = NULL;
    char *tmp_result = NULL;
    char *tmp_a = NULL;
    char *tmp_b = NULL;

    if (!a || !b) {
        return NULL; // Invalid input
    }

    if (!strcmp(a->value, "0.0") || !strcmp(b->value, "0,0")) {
        char *tmp_value = a->value;
        a->value = (char *) malloc(4);
        if (!a->value) {
            a->value = tmp_value;
            return NULL;
        }
        free(tmp_value);
        strcpy(a->value, "0.0");
        a->size = strlen(a->value);
        a->scale = 1;
        a->sign = 0;
        return a;
    }

    tmp_a = (char *)malloc(a->size); // 用于存储a的十进制值字符串的临时缓冲区
    tmp_b = (char *)malloc(b->size); // 用于存储b的十进制值字符串的临时缓冲区
    char *value_a_p = a->value; // 指向a的十进制值字符串的指针
    char *value_b_p = b->value; // 指向b的十进制值字符串的指针
    char *tmp_a_p = tmp_a;
    char *tmp_b_p = tmp_b;
    if (!tmp_a || !tmp_b) {
        goto ERROR;
    }
    while (*value_a_p) {
        if (*value_a_p != '.') {
            *tmp_a_p++ = *value_a_p; // 将a的十进制值字符串中的数字字符复制到tmp_a中，跳过小数点
        }
        value_a_p++;
    }
    *tmp_a_p = '\0'; // 在tmp_a的末尾添加字符串终止符
    while (*value_b_p) {
        if (*value_b_p != '.') {
            *tmp_b_p++ = *value_b_p; // 将b的十进制值字符串中的数字字符复制到tmp_b中，跳过小数点
        }
        value_b_p++;
    }
    *tmp_b_p = '\0'; // 在tmp_b的末尾添加字符串终止符


    int size_a = strlen(tmp_a); // 获取a的整数部分和小数部分的总位数
    int size_b = strlen(tmp_b); // 获取b的整数部分和小数部分的总位数
    int scale_a = a->scale; // 获取a的小数部分的位数
    int scale_b = b->scale; // 获取b的小数部分的位数

    value_a_p = --tmp_a_p; // 将指针重新指向tmp_a的最后一个字符
    value_b_p = --tmp_b_p; // 将指针重新指向tmp_b的最后一个字符

    result = decimal_alloc_string("0"); // 创建一个新的decimal_t实例来存储乘积的结果，初始值为0
    if (!result) {
        goto ERROR;
    }
    tmp_decimal = (decimal_t *) malloc(sizeof(decimal_t));
    if (!tmp_decimal) {
        goto ERROR;
    }
    tmp_decimal->value = (char *) malloc(size_a + size_b + 5);
    if (!tmp_decimal->value) {
        goto ERROR;
    }

    tmp_result = (char *)malloc(size_a + size_b + 5); // 用于存储当前位乘积的临时字符串
    if (!tmp_result) {
        goto ERROR;
    }


    while (value_b_p >= tmp_b) { // 外层循环遍历b的每一位
        char *tmp_result_p_start = tmp_result + size_a; // 临时结果字符串的起始位置，预留一个字符用于小数点
        char *tmp_result_p = tmp_result_p_start;

        int digit_b = (*value_b_p >= '0' && *value_b_p <= '9') ? (*value_b_p - '0') : 0; // 获取b当前位的数字
        char carry = 0; // 用于存储乘积的进位
        char *value_a_p_inner = value_a_p; // 内层循环中用于遍历a的指针
        if (digit_b != 0) {
            while (value_a_p_inner >= tmp_a) { // 内层循环遍历a的每一位
                int digit_a = (*value_a_p_inner >= '0' && *value_a_p_inner <= '9') ? (*value_a_p_inner - '0') : 0; // 获取a当前位的数字
                int product = digit_a * digit_b + carry; // 计算当前位的乘积加上进位
                *tmp_result_p = (product % 10) + '0'; // 将当前位的结果转换为字符并存储在临时结果中
                carry = product / 10; // 更新进位为当前位乘积除以10的商
                tmp_result_p--; // 将临时结果指针向左移动以准备存储下一位的结果
                value_a_p_inner--; // 将a的指针向左移动以处理下一位
            }

            if (carry > 0) { // 如果最后还有进位，则将其存储在临时结果中
                *tmp_result_p = carry + '0'; // 将进位转换为字符并存储在临时结果中
                tmp_result_p--; // 将临时结果指针向左移动以准备存储下一位的结果
                tmp_result_p = tmp_result_p_start + 1;
            } else {
                for (int i = 0; i < size_a; i++)
                {
                    tmp_result[i] = tmp_result[i+1];
                }
                tmp_result_p = tmp_result_p_start;
            }
    
            int zeros_to_append = (size_b - 1 - (value_b_p - tmp_b)); // 根据b当前位的位置计算需要在当前位乘积后面添加的零的数量
            for (int i = 0; i < zeros_to_append; i++) {
                *(tmp_result_p++) = '0'; // 在当前位乘积后面添加零
            }
            *(tmp_result_p++) = '\0';
        } else {
            strcpy(tmp_result, "0");
        }
        // 将当前位乘积与之前的结果进行加法操作
        decimal_static_string(tmp_decimal, tmp_result);
        decimal_add(result, tmp_decimal);
        value_b_p--;
    }

    memset(tmp_result, 0, size_a + size_b + 5);
    int decimal_scale = result->size - (a->scale + b->scale) - 1;
    char *tmp_result_p = tmp_result;
    tmp_a_p = result->value;

    // 将之前移除的小数点放置到指定位置
    while (1)
    {
        if (!(--decimal_scale)) {
            *(tmp_result_p++) = '.';
            continue;
        }
        if (*tmp_a_p == '.') {
            tmp_a_p++;
            break;
        }
        *(tmp_result_p++) = *(tmp_a_p++);
    }
    *tmp_result_p = '\0';

    char *decimal_end = strchr(tmp_result, '.'); // 查找结果字符串中的小数点位置
    decimal_end += 1; // 将指针移动到小数点后第一个数字的位置
    char *tmp_end = tmp_result + strlen(tmp_result) - 1; // 结果字符串的末尾位置
    while (tmp_end > decimal_end && *tmp_end == '0') {
        *tmp_end = '\0'; // 将末尾的零字符替换为字符串终止符
        tmp_end--; // 将末尾指针向左移动以继续检查下一个字符
    }


    free(a->value);
    a->value = (char *) malloc(result->size + 2);
    if (!a->value) {
        goto ERROR;
    }
    strcpy(a->value, tmp_result);

    a->size = result->size + 1;
    a->scale = a->scale + b->scale;
    a->sign  = a->sign == b->sign ? 0 : 1;

    ret_dml = a;
    

free_data_out:
    if (result)
        decimal_free(result);
    if (tmp_decimal)
        decimal_free(tmp_decimal);
    if (tmp_a)
        free(tmp_a); 
    if (tmp_b)
        free(tmp_b);
    if (tmp_result)
        free(tmp_result);
    return ret_dml;

ERROR:
    ret_dml = NULL;
    goto free_data_out;
}


decimal_t *decimal_div(decimal_t *a, decimal_t *b);




/**
 * @brief 释放decimal_t实例占用的内存。
 * @param d 要释放的decimal_t实例。
 * @note 该函数会释放decimal_t结构体本身以及其中的value字符串所占用的内存。
 */
void inline decimal_free(decimal_t *d)
{
    if (d) {
        if (d->value) {
            free(d->value); // 释放十进制值字符串的内存
        }
        free(d); // 释放decimal结构体的内存
    }
}


/**
 * @brief 比较两个decimal_t实例的大小。
 * @param a decimal_t实例一。
 * @param b decimal_t实例二。
 * @return 返回0：等于，1：不等于。
 */
int decimal_compare(decimal_t *a, decimal_t *b)
{
    if (!a || !b) return 1;

    if (a->sign == b->sign && !strcmp(a->value, b->value)) return 0;

    return 1;
}


/**
 * @brief 将decimal_t实例转换为 双精度浮点数(double) 表示形式。
 * @param d 要转换的decimal_t实例。
 * @param value 存储双精度浮点数的地址。
 * @return 返回0：失败，n：要打印的精度数。
 */
int decimal_to_double(decimal_t *d, double *value)
{
    if (!d || !value) {
        return 0;
    }

    double tmp_result = 0;
    char *tmp_value = d->value;
    int scale_d = d->scale;
    int double_len = d->size - (d->scale > 15 ? (d->scale - 15) : 0) - 1; // double只能保留小数点后十五位

    // 先忽略小数点
    while (double_len > 0)
    {
        if (*tmp_value == '.') {
            tmp_value++;
        }
        tmp_result += (*(tmp_value++) - '0') * pow(10, --double_len);
    }

    while (scale_d > 0)
    {
        tmp_result = tmp_result / 10;
        scale_d--;
    }
    *value = tmp_result;

    return d->scale;
}


/**
 * @brief 将decimal_t实例转换为字符串表示形式。
 * @param buf 用于存储转换结果的缓冲区。调用者需要确保该缓冲区足够大以容纳结果字符串。
 * @param d 要转换的decimal_t实例。
 * @return 表示该十进制值的字符串。调用者负责释放返回的字符串内存。
 */
char *decimal_to_string(char *buf, decimal_t *d)
{
    if (!d || !buf) {
        return NULL; // Invalid input
    }
    
    char *result = buf; // 将结果字符串的起始位置保存到result变量中，以便在函数末尾返回
    
    if (d->sign) {
        *result++ = '-'; // 如果十进制数是负数，则在字符串开头添加负号
    }

    strcpy(result, d->value); // 将十进制数的字符串表示复制到缓冲区中
    return buf; // 返回结果字符串的起始位置
}
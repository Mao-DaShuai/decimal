#include "decimal.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#define MAX_PRECISION 50

/* ── 底层字符串辅助函数（整数数字字符串，不含小数点）── */

/* 比较两个整数数字字符串（除 "0" 外无前导零） */
static int cmp_int_str(const char *a, const char *b)
{
    int la = (int)strlen(a), lb = (int)strlen(b);
    if (la != lb) return (la > lb) ? 1 : -1;
    return strcmp(a, b);
}

/* 从 a 中减去 b（原地修改 a）。a 必须 >= b。 */
static void sub_int_str(char *a, const char *b)
{
    int la = (int)strlen(a), lb = (int)strlen(b);
    int i = la - 1, j = lb - 1;
    int borrow = 0;
    while (j >= 0 || borrow) {
        int da = a[i] - '0';
        int db = (j >= 0) ? (b[j] - '0') : 0;
        int d = da - db - borrow;
        if (d < 0) { d += 10; borrow = 1; }
        else        borrow = 0;
        a[i] = (char)(d + '0');
        i--; j--;
    }
    /* 去掉前导零（至少保留一位） */
    int start = 0;
    while (a[start] == '0' && a[start + 1]) start++;
    if (start > 0) memmove(a, a + start, la - start + 1);
}

/* 将整数数字字符串乘以一位数字 (1-9)，结果写入 product。返回乘积字符串的长度。 */
static int mul_by_digit(const char *num, int digit, char *product)
{
    int len = (int)strlen(num);
    int carry = 0;
    int p = 0;
    char tmp[1024];
    for (int i = len - 1; i >= 0; i--) {
        int prod = (num[i] - '0') * digit + carry;
        tmp[p++] = (char)((prod % 10) + '0');
        carry = prod / 10;
    }
    while (carry) {
        tmp[p++] = (char)((carry % 10) + '0');
        carry /= 10;
    }
    /* 反转结果到 product */
    for (int i = 0; i < p; i++) {
        product[i] = tmp[p - 1 - i];
    }
    product[p] = '\0';
    return p;
}

/* 去掉 val 中的小数点，将纯数字写入 out。返回 scale（小数点后的位数）。 */
static int remove_dot(const char *val, char *out)
{
    int out_idx = 0, dot = -1;
    for (int i = 0; val[i]; i++) {
        if (val[i] == '.') dot = i;
        else out[out_idx++] = val[i];
    }
    out[out_idx] = '\0';
    if (dot >= 0) return (int)(strlen(val) - dot - 1);
    return 0;
}

/* 长除法：dividend / divisor，产生足够 digits 达到 'precision' 位小数。
   返回新分配的字符串，包含完整商（整数部分 + 小数部分），调用者需自行放置小数点。 */
static char *long_division(const char *dividend, const char *divisor, int precision)
{
    int dvd_len = (int)strlen(dividend);
    int total   = dvd_len + precision;          /* 商的总位数 */

    /* 调整后的被除数 = dividend 末尾补 'precision' 个零 */
    char *adjusted = (char *)malloc((size_t)total + 1);
    if (!adjusted) return NULL;
    strcpy(adjusted, dividend);
    memset(adjusted + dvd_len, '0', (size_t)precision);
    adjusted[total] = '\0';

    char *quotient = (char *)malloc((size_t)total + 1);
    if (!quotient) { free(adjusted); return NULL; }

    char remainder[1024];
    int  rem_len = 0;
    remainder[0] = '\0';

    int qp = 0;
    for (int i = 0; i < total; i++) {
        /* 下移一位数字 */
        remainder[rem_len++] = adjusted[i];
        remainder[rem_len]   = '\0';

        /* 去掉余数的前导零 */
        int s = 0;
        while (remainder[s] == '0' && remainder[s + 1]) s++;
        if (s > 0) {
            rem_len -= s;
            memmove(remainder, remainder + s, (size_t)rem_len + 1);
        }

        int q = 0;
        if (cmp_int_str(remainder, divisor) >= 0) {
            char prod[1024];
            /* 从 9 到 1 尝试，找到最大的 digit 使得 digit * divisor <= remainder */
            for (int d = 9; d >= 1; d--) {
                mul_by_digit(divisor, d, prod);
                if (cmp_int_str(prod, remainder) <= 0) {
                        q = d;
                        sub_int_str(remainder, prod);
                        rem_len = (int)strlen(remainder); /* sub_int_str 可能缩短了字符串 */
                        break;
                }
            }
        }
        quotient[qp++] = (char)(q + '0');
    }
    quotient[qp] = '\0';

    free(adjusted);
    return quotient;
}


/* ─────────────────────────── 公开 API ─────────────────────────── */

decimal_t *decimal_alloc_int64(int64_t value)
{
    decimal_t *d = (decimal_t *)malloc(sizeof(decimal_t));
    if (!d) return NULL;

    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%lld",
             (long long)(value < 0 ? -value : value));

    int size = (int)strlen(buffer);
    d->value = (char *)malloc((size_t)(size + 3)); /* 数字部分 + ".0" + NUL */
    if (!d->value) { free(d); return NULL; }

    strcpy(d->value, buffer);
    strcat(d->value, ".0");

    d->size  = (int)strlen(d->value);
    d->scale = 1;
    d->sign  = (value < 0) ? 1 : 0;
    return d;
}

decimal_t *decimal_alloc_double(double value)
{
    decimal_t *d = (decimal_t *)malloc(sizeof(decimal_t));
    if (!d) return NULL;

    char buffer[64];
    int precision = 0;
    double tmp = value < 0 ? -value : value;
    while (precision <= 15 && fabs(tmp - round(tmp)) > 1e-15) {
        precision++;
        tmp *= 10.0;
    }
    snprintf(buffer, sizeof(buffer), "%.*f", precision, value);

    /* 去掉末尾多余的零 */
    char *dp = strchr(buffer, '.');
    if (dp) {
        char *end = buffer + strlen(buffer) - 1;
        while (end > dp && *end == '0') *end-- = '\0';
        if (end == dp) { dp[1] = '0'; dp[2] = '\0'; }
    }

    /* 构造最终值：保证有小数点 */
    char final[70];
    if (strchr(buffer, '.')) {
        strcpy(final, buffer);
    } else {
        strcpy(final, buffer);
        strcat(final, ".0");
    }

    d->value = strdup(final);
    if (!d->value) { free(d); return NULL; }

    d->size  = (int)strlen(d->value);
    dp       = strchr(d->value, '.');
    d->scale = dp ? (int)strlen(dp + 1) : 1;
    d->sign  = (value < 0) ? 1 : 0;
    return d;
}

decimal_t *decimal_alloc_string(const char *str)
{
    if (!str) return NULL;
    decimal_t *d = (decimal_t *)malloc(sizeof(decimal_t));
    if (!d) return NULL;

    const char *src = (*str == '-') ? str + 1 : str;

    d->value = (char *)malloc(strlen(src) + 4); /* src + ".0" + NUL */
    if (!d->value) { free(d); return NULL; }

    strcpy(d->value, src);
    if (!strchr(d->value, '.')) strcat(d->value, ".0");

    d->size  = (int)strlen(d->value);
    char *dp = strchr(d->value, '.');
    d->scale = dp ? (int)strlen(dp + 1) : 1;
    d->sign  = (*str == '-') ? 1 : 0;
    return d;
}

decimal_t *decimal_static_string(decimal_t *dml, const char *str)
{
    if (!dml || !str) return NULL;

    const char *src = (*str == '-') ? str + 1 : str;
    int src_len = (int)strlen(src);

    if (!dml->value) {
        fprintf(stderr, "static decimal_t.value not allocated!\n");
        return NULL;
    }

    /* 检查缓冲区是否足够大（调用者应已分配足够空间） */
    strcpy(dml->value, src);
    if (!strchr(dml->value, '.')) strcat(dml->value, ".0");

    dml->size  = (int)strlen(dml->value);
    char *dp = strchr(dml->value, '.');
    dml->scale = dp ? (int)strlen(dp + 1) : 1;
    dml->sign  = (*str == '-') ? 1 : 0;
    return dml;
}

decimal_t *decimal_copy(decimal_t *dst, decimal_t *src)
{
    assert(dst && src);
    if (dst->value) free(dst->value);
    dst->value = strdup(src->value);
    dst->scale = src->scale;
    dst->sign  = src->sign;
    dst->size  = src->size;
    return dst;
}

/* ──────────────────── 去掉末尾零 / 更新元数据 ──────────── */

static void trim_decimal(decimal_t *d)
{
    char *dp = strchr(d->value, '.');
    if (!dp) return; /* 不应发生 */

    int len = (int)strlen(d->value);
    char *end = d->value + len - 1;
    while (end > dp && *end == '0') *end-- = '\0';

    /* 更新 size 和 scale */
    d->size  = (int)(end - d->value + 1);
    d->scale = dp ? (int)strlen(dp + 1) : 0;
    /* 如果小数点后没有数字了，恢复 ".0"（"123." 很难看） */
    if (d->scale == 0) {
        dp[1] = '0'; dp[2] = '\0';
        d->size += 1;
        d->scale = 1;
    }
}

/* ──────────────────── 加 / 减 ──────────────────── */

static void align_values(const char *va, int sa, const char *vb, int sb,
                         char *out_a, char *out_b, int *max_scale, int *max_int)
{
    /* 确定最大小数位数 */
    int ms = (sa > sb) ? sa : sb;
    *max_scale = ms;
    int ia = (int)strlen(va) - sa - 1; /* a 的整数位数 */
    int ib = (int)strlen(vb) - sb - 1; /* b 的整数位数 */
    int mi = (ia > ib) ? ia : ib;
    *max_int = mi;

    /* 构造对齐的字符串：整数部分 . 小数部分（两者格式相同） */
    /* out_a, out_b 至少需要 mi + ms + 3 字节 */

    char *p = out_a;
    /* 整数部分补前导零 */
    for (int i = 0; i < mi - ia; i++) *p++ = '0';
    /* 复制 va 的整数数字 */
    const char *q = va;
    while (*q && *q != '.') *p++ = *q++;
    if (*q == '.') q++; /* 跳过小数点 */
    /* 小数点 */
    *p++ = '.';
    /* 复制小数部分的数字 */
    for (int i = 0; i < ms; i++) {
        if (i < sa && *q) *p++ = *q++;
        else *p++ = '0';
    }
    *p = '\0';

    p = out_b;
    for (int i = 0; i < mi - ib; i++) *p++ = '0';
    q = vb;
    while (*q && *q != '.') *p++ = *q++;
    if (*q == '.') q++;
    *p++ = '.';
    for (int i = 0; i < ms; i++) {
        if (i < sb && *q) *p++ = *q++;
        else *p++ = '0';
    }
    *p = '\0';
}

decimal_t *decimal_add(decimal_t *a, decimal_t *b)
{
    if (!a || !b) return NULL;

    /* 快速判断：同号 → 加法；异号 → 减法 */
    int same_sign = (a->sign == b->sign);

    /* 将两个值对齐到相同宽度 */
    char buf_a[1024], buf_b[1024];
    int max_scale, max_int;
    align_values(a->value, a->scale, b->value, b->scale,
                 buf_a, buf_b, &max_scale, &max_int);

    int total = max_int + max_scale + 4; /* 多留一个字节给可能的进位 */
    char *result = (char *)calloc((size_t)total, 1);
    if (!result) return NULL;

    int pos = max_int + max_scale + 2; /* 从最右侧位置开始 */
    result[pos] = '\0'; pos--;

    int borrow = 0, carry = 0;

    if (!same_sign) {
        /* ── 减法 ── */
        /* 始终计算 |a| - |b|。如果 |b| > |a|，标记 neg_swap 并交换。 */
        int neg_swap = 0;
        /* 先比较对齐后的整数部分 */
        char int_a[512], int_b[512];
        {
            const char *pa = buf_a, *pb = buf_b;
            int i;
            for (i = 0; i < max_int; i++) { int_a[i] = *pa++; int_b[i] = *pb++; }
            int_a[i] = int_b[i] = '\0';
        }
        int cmp = cmp_int_str(int_a, int_b);
        if (cmp == 0) {
            /* 比较完整字符串（包括小数部分） */
            cmp = strcmp(buf_a, buf_b);
        }
        const char *big, *small;
        if (cmp < 0) {
            neg_swap = 1;
            big   = buf_b;
            small = buf_a;
        } else {
            big   = buf_a;
            small = buf_b;
        }

        int len_big = (int)strlen(big);
        int i = len_big - 1, j = (int)strlen(small) - 1;
        while (j >= 0 || borrow) {
            int db = (big[i] == '.') ? '.' : (big[i] - '0');
            int ds = (j >= 0 && small[j] != '.') ? (small[j] - '0') : 0;
            if (big[i] == '.') {
                result[pos--] = '.';
                i--; if (j >= 0) j--;
                continue;
            }
            int d = db - ds - borrow;
            if (d < 0) { d += 10; borrow = 1; }
            else         borrow = 0;
            result[pos--] = (char)(d + '0');
            i--; j--;
        }
        /* 复制 big 中剩余的数字 */
        while (i >= 0) {
            if (big[i] == '.') { result[pos--] = '.'; i--; continue; }
            result[pos--] = big[i--];
        }

        /* 确定最终符号：result = |big| - |small|，符号取较大值对应的符号 */
        a->sign = neg_swap ? b->sign : a->sign;
    } else {
        /* ── 加法 ── */
        int i = (int)strlen(buf_a) - 1, j = (int)strlen(buf_b) - 1;
        while (i >= 0 || j >= 0 || carry) {
            if (i >= 0 && buf_a[i] == '.') { result[pos--] = '.'; i--; j--; continue; }
            int da = (i >= 0) ? (buf_a[i] - '0') : 0;
            int db = (j >= 0) ? (buf_b[j] - '0') : 0;
            int s  = da + db + carry;
            carry  = s / 10;
            result[pos--] = (char)((s % 10) + '0');
            i--; j--;
        }
    }

    /* result 中有效数字从 pos+1 开始 */
    char *num_start = result + pos + 1;

    /* 去掉前导零 */
    while (*num_start == '0' && *(num_start + 1) != '.' && *(num_start + 1) != '\0')
        num_start++;

    /* 替换 a 的值 */
    free(a->value);
    a->value = strdup(num_start);
    free(result);
    if (!a->value) return NULL;

    trim_decimal(a);
    return a;
}

decimal_t *decimal_sub(decimal_t *a, decimal_t *b)
{
    if (!a || !b) return NULL;

    decimal_t *neg_b = (decimal_t *)malloc(sizeof(decimal_t));
    if (!neg_b) return NULL;
    neg_b->value = strdup(b->value);
    if (!neg_b->value) { free(neg_b); return NULL; }
    neg_b->size  = b->size;
    neg_b->scale = b->scale;
    neg_b->sign  = !b->sign;

    decimal_t *ret = decimal_add(a, neg_b);
    decimal_free(neg_b);
    return ret;
}

/* ──────────────────── 乘法 ──────────────────── */

decimal_t *decimal_mul(decimal_t *a, decimal_t *b)
{
    if (!a || !b) return NULL;
    if (!strcmp(a->value, "0.0") || !strcmp(b->value, "0.0")) {
        free(a->value);
        a->value = strdup("0.0");
        a->size  = 3;
        a->scale = 1;
        a->sign  = 0;
        return a;
    }

    int scale_a = a->scale, scale_b = b->scale;

    /* 去掉小数点 */
    char a_digits[512], b_digits[512];
    remove_dot(a->value, a_digits);
    remove_dot(b->value, b_digits);

    int size_a = (int)strlen(a_digits);
    int size_b = (int)strlen(b_digits);

    /* 乘积累加器（整数数字字符串） */
    char *prod = (char *)calloc((size_t)(size_a + size_b + 2), 1);
    if (!prod) return NULL;
    prod[0] = '0';   /* 从零开始 */

    /* 遍历 b 的每一位（从右到左） */
    for (int bi = size_b - 1; bi >= 0; bi--) {
        int digit_b = b_digits[bi] - '0';
        if (digit_b == 0) continue;

        /* 用 digit_b 乘以 a_digits */
        char row[1024];
        int rlen = mul_by_digit(a_digits, digit_b, row);

        /* 移位：按位置补零 */
        int shift = size_b - 1 - bi;
        for (int s = 0; s < shift; s++) row[rlen++] = '0';
        row[rlen] = '\0';

        /* 将 row 加到 prod */
        /* prod 可能有前导零，需要对齐 */
        int plen = (int)strlen(prod);
        int max_len = (plen > rlen) ? plen : rlen;
        char p_buf[2048], r_buf[2048];
        /* 将两个数字前补零到相同长度 */
        for (int i = 0; i < max_len - plen; i++) p_buf[i] = '0';
        strcpy(p_buf + max_len - plen, prod);
        for (int i = 0; i < max_len - rlen; i++) r_buf[i] = '0';
        strcpy(r_buf + max_len - rlen, row);

        int carry = 0;
        for (int i = max_len - 1; i >= 0; i--) {
            int s = (p_buf[i] - '0') + (r_buf[i] - '0') + carry;
            p_buf[i] = (char)((s % 10) + '0');
            carry = s / 10;
        }
        /* 处理最后的进位 */
        if (carry) {
            memmove(p_buf + 1, p_buf, (size_t)max_len + 1);
            p_buf[0] = (char)(carry + '0');
        }
        strcpy(prod, p_buf);
    }

    /* 放置小数点：结果的小数位数 = scale_a + scale_b */
    int result_scale = scale_a + scale_b;
    int prod_len = (int)strlen(prod);

    /* 构造最终值字符串 */
    char final[2048];
    int int_len = prod_len - result_scale;
    if (int_len <= 0) {
        /* 需要补前导零：例如 prod="5", scale=2 → "0.05" */
        final[0] = '0';
        final[1] = '.';
        for (int i = 0; i < -int_len; i++) final[2 + i] = '0';
        strcpy(final + 2 + (-int_len), prod);
    } else {
        strncpy(final, prod, (size_t)int_len);
        final[int_len] = '.';
        strcpy(final + int_len + 1, prod + int_len);
    }

    /* 去掉末尾多余的零 */
    char *dp = strchr(final, '.');
    if (dp) {
        char *end = final + strlen(final) - 1;
        while (end > dp && *end == '0') *end-- = '\0';
        if (*dp != '\0' && dp[1] == '\0') { dp[1] = '0'; dp[2] = '\0'; }
    }

    free(a->value);
    a->value = strdup(final);
    free(prod);
    if (!a->value) return NULL;

    a->sign  = (a->sign == b->sign) ? 0 : 1;
    trim_decimal(a);
    return a;
}

/* ──────────────────── 除法 ──────────────────── */

decimal_t *decimal_div(decimal_t *a, decimal_t *b)
{
    decimal_t *ret_dml = NULL;

    if (!a || !b) return NULL;

    /* 检查除数为零 */
    {
        char tmp[512];
        remove_dot(b->value, tmp);
        int all_zero = 1;
        for (int i = 0; tmp[i]; i++) {
            if (tmp[i] != '0') { all_zero = 0; break; }
        }
        if (all_zero) return NULL; /* 除零错误 */
    }
    /* 检查 a 是否为零 */
    {
        char tmp[512];
        remove_dot(a->value, tmp);
        int all_zero = 1;
        for (int i = 0; tmp[i]; i++) {
            if (tmp[i] != '0') { all_zero = 0; break; }
        }
        if (all_zero) {
            /* 0 / 任何数 = 0 */
            free(a->value);
            a->value = strdup("0.0");
            a->size  = 3;
            a->scale = 1;
            a->sign  = 0;
            return a;
        }
    }

    /* 去掉两个操作数的小数点 */
    char a_raw[512], b_raw[512];
    int a_scale = remove_dot(a->value, a_raw);
    int b_scale = remove_dot(b->value, b_raw);

    /* 期望精度 */
    int precision = MAX_PRECISION;

    /* 长除法：dividend = a_raw, divisor = b_raw
       计算 a_raw / b_raw 并达到 'precision' 位小数，
       考虑 a = a_raw / 10^a_scale, b = b_raw / 10^b_scale。
       所以 a/b = (a_raw / b_raw) * 10^(b_scale - a_scale)。
       精确到 'precision' 位小数：
         effective_dividend = a_raw * 10^(precision + b_scale - a_scale)
       如果 (precision + b_scale - a_scale) < 0，则截断为 0
         （即小数位数少于目标精度，直接计算结果，后面补零）。
    */
    int padding = precision + b_scale - a_scale;
    if (padding < 0) {
        /* 需要先将 a_raw 除以 10^(-padding)，这相当于减少整数部分。
           为简单起见，直接截断为 0 —— 结果的小数位数会少于请求值，
           但这是在不丢失数据前提下的最佳结果。 */
        padding = 0;
    }

    char *quotient = long_division(a_raw, b_raw, padding);
    if (!quotient) return NULL;

    int qlen = (int)strlen(quotient);

    /* 构造最终字符串，放置小数点 */
    char final[2048];
    int int_len = qlen - padding; /* 整数部分的位数 */

    if (int_len <= 0) {
        /* 例如 quotient = "005", padding = 5 → "0.00005" */
        final[0] = '0';
        final[1] = '.';
        for (int i = 0; i < -int_len; i++) final[2 + i] = '0';
        strcpy(final + 2 + (-int_len), quotient);
    } else {
        /* 找到整数部分的起始位置，跳过前导零 */
        const char *qp = quotient;
        int trimmed = 0;
        while (*qp == '0' && trimmed < int_len - 1) { qp++; trimmed++; }
        int new_int_len = int_len - trimmed;
        strncpy(final, qp, (size_t)new_int_len);
        final[new_int_len] = '.';
        strcpy(final + new_int_len + 1, quotient + int_len);
        int_len = new_int_len; /* 为后续使用更新 */
    }

    free(quotient);

    /* 去掉末尾多余的零 */
    char *dp = strchr(final, '.');
    if (dp) {
        char *end = final + strlen(final) - 1;
        while (end > dp && *end == '0') *end-- = '\0';
        if (dp[1] == '\0') { dp[1] = '0'; dp[2] = '\0'; }
    }

    free(a->value);
    a->value = strdup(final);
    if (!a->value) goto DML_DIV_ERROR;

    a->sign = (a->sign == b->sign) ? 0 : 1;
    trim_decimal(a);
    return a;

DML_DIV_ERROR:
    ret_dml = NULL;
    return ret_dml;
}

/* ──────────────────── 比较 ──────────────────── */

int decimal_compare(decimal_t *a, decimal_t *b)
{
    assert(a && b);

    /* 检查符号 */
    if (a->sign != b->sign)
        return (a->sign == 0) ? 1 : -1; /* 正数 > 负数 */

    /* 同号 → 直接比较对齐后的数字字符串（避免使用堆内存） */
    char a_digits[512], b_digits[512];
    int a_scale = remove_dot(a->value, a_digits);
    int b_scale = remove_dot(b->value, b_digits);
    int a_int    = (int)strlen(a_digits) - a_scale;
    int b_int    = (int)strlen(b_digits) - b_scale;
    int max_int  = (a_int > b_int) ? a_int : b_int;
    int max_scale = (a_scale > b_scale) ? a_scale : b_scale;

    char a_aligned[1024], b_aligned[1024];
    char *p = a_aligned;
    for (int i = 0; i < max_int - a_int; i++) *p++ = '0';
    strcpy(p, a_digits);
    p += strlen(a_digits);
    for (int i = a_scale; i < max_scale; i++) *p++ = '0';
    *p = '\0';

    p = b_aligned;
    for (int i = 0; i < max_int - b_int; i++) *p++ = '0';
    strcpy(p, b_digits);
    p += strlen(b_digits);
    for (int i = b_scale; i < max_scale; i++) *p++ = '0';
    *p = '\0';

    int cmp = strcmp(a_aligned, b_aligned);
    if (cmp == 0) return 0;
    return (a->sign == 0) ? ((cmp > 0) ? 1 : -1) : ((cmp > 0) ? -1 : 1);
}

/* ──────────────────── 转换辅助函数 ──────────────────── */

int decimal_to_double(decimal_t *d, double *value)
{
    if (!d || !value) return 0;

    double tmp_result = 0;
    char *tmp_value = d->value;
    int scale_d = d->scale;
    int double_len = d->size - (scale_d > 15 ? (scale_d - 15) : 0) - 1;

    while (double_len > 0) {
        if (*tmp_value == '.') { tmp_value++; continue; }
        tmp_result += (double)(*(tmp_value++) - '0') * pow(10.0, (double)(--double_len));
    }

    while (scale_d > 0) {
        tmp_result /= 10.0;
        scale_d--;
    }
    if (d->sign) tmp_result = -tmp_result;
    *value = tmp_result;
    return d->scale;
}

void decimal_free(decimal_t *d)
{
    if (d) {
        free(d->value);
        free(d);
    }
}

char *decimal_to_string(char *buf, decimal_t *d)
{
    if (!d || !buf) return NULL;
    char *p = buf;
    if (d->sign) *p++ = '-';
    strcpy(p, d->value);
    return buf;
}
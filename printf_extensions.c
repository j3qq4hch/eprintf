#include "printf_extensions.h"
#include <string.h>

/* Вспомогательная функция для вывода числа */
static void print_number(uint32_t n, int sign, char **buffer, size_t *size, 
                          format_flags_t flags, int base, const char *digits) {
    char tmp[33];
    int i = 0;
    
    do {
        tmp[i++] = digits[n % base];
        n /= base;
    } while (n > 0);
    
    int num_digits = i;
    int sign_char = (sign < 0) ? '-' : 0;
    int total_len = num_digits + (sign_char ? 1 : 0);
    int padding = (flags.width > total_len) ? flags.width - total_len : 0;
    char pad_char = flags.zero_pad ? '0' : ' ';
    
    if (!flags.left_align) {
        if (flags.zero_pad && sign_char) {
            buf_putc(buffer, size, sign_char);
            sign_char = 0;
        }
        buf_repeat_char(buffer, size, pad_char, padding);
    }
    
    if (sign_char) {
        buf_putc(buffer, size, sign_char);
    }
    
    while (i > 0) {
        buf_putc(buffer, size, tmp[--i]);
    }
    
    if (flags.left_align) {
        buf_repeat_char(buffer, size, ' ', padding);
    }
}

/* X-макрос для всех форматов - только спецификаторы */
// Cпецификаторы форматов должны располагаться по убыванию длины

#define FORMAT_TABLE \
    X(io) \
    X(s)  \
    X(c)  \
    X(i)  \
    X(u)  \
    X(x)  \
    X(b)  \
    X(p)

/* Макрос для генерации имени обработчика из спецификатора */
#define HANDLER_NAME(spec)  spec##_handler

/* Генерация объявлений всех обработчиков */
#define X(spec) static void HANDLER_NAME(spec)(va_list *args, char **buffer, size_t *size, format_flags_t flags);
FORMAT_TABLE
#undef X

/* Генерация массива форматов */
const format_extension_t FORMATS[] = {
    #define X(spec) {#spec, HANDLER_NAME(spec)},
    FORMAT_TABLE
    #undef X
};

const int FORMATS_COUNT = sizeof(FORMATS) / sizeof(FORMATS[0]);

/* Реализации всех обработчиков */

/* %s - вывод строки */
static void s_handler(va_list *args, char **buffer, size_t *size, format_flags_t flags) {
    const char *str = va_arg(*args, const char*);
    if (!str) str = "(null)";
    
    int len = (int)strlen(str);
    int padding = (flags.width > len) ? flags.width - len : 0;
    
    if (!flags.left_align && padding > 0) {
        buf_repeat_char(buffer, size, ' ', padding);
    }
    buf_puts(buffer, size, str, len);
    if (flags.left_align && padding > 0) {
        buf_repeat_char(buffer, size, ' ', padding);
    }
}

/* %c - вывод символа */
static void c_handler(va_list *args, char **buffer, size_t *size, format_flags_t flags) {
    char c = (char)va_arg(*args, int);
    int padding = (flags.width > 1) ? flags.width - 1 : 0;
    
    if (!flags.left_align && padding > 0) {
        buf_repeat_char(buffer, size, ' ', padding);
    }
    buf_putc(buffer, size, c);
    if (flags.left_align && padding > 0) {
        buf_repeat_char(buffer, size, ' ', padding);
    }
}

/* %i - вывод целого числа */
static void i_handler(va_list *args, char **buffer, size_t *size, format_flags_t flags) {
    int32_t n = va_arg(*args, int32_t);
    int sign = (n < 0) ? -1 : 1;
    uint32_t un = (n < 0) ? (uint32_t)(-n) : (uint32_t)n;
    print_number(un, sign, buffer, size, flags, 10, "0123456789");
}

/* %u - вывод беззнакового целого */
static void u_handler(va_list *args, char **buffer, size_t *size, format_flags_t flags) {
    uint32_t n = va_arg(*args, uint32_t);
    print_number(n, 0, buffer, size, flags, 10, "0123456789");
}

/* %x - вывод шестнадцатеричного числа */
static void x_handler(va_list *args, char **buffer, size_t *size, format_flags_t flags) {
    uint32_t n = va_arg(*args, uint32_t);
    int prefix_len = 2;
    uint32_t tmp = n;
    int num_digits = 0;
    do {
        num_digits++;
        tmp /= 16;
    } while (tmp > 0);
    
    int total_len = num_digits + prefix_len;
    int padding = (flags.width > total_len) ? flags.width - total_len : 0;
    
    if (!flags.left_align) {
        buf_repeat_char(buffer, size, flags.zero_pad ? '0' : ' ', padding);
    }
    buf_puts(buffer, size, "0x", 2);
    print_number(n, 0, buffer, size, flags, 16, "0123456789abcdef");
    if (flags.left_align) {
        buf_repeat_char(buffer, size, ' ', padding);
    }
}

/* %b - вывод двоичного числа */
static void b_handler(va_list *args, char **buffer, size_t *size, format_flags_t flags) {
    uint32_t n = va_arg(*args, uint32_t);
    int prefix_len = 2;
    uint32_t tmp = n;
    int num_digits = 0;
    do {
        num_digits++;
        tmp /= 2;
    } while (tmp > 0);
    
    int total_len = num_digits + prefix_len;
    int padding = (flags.width > total_len) ? flags.width - total_len : 0;
    
    if (!flags.left_align) {
        buf_repeat_char(buffer, size, flags.zero_pad ? '0' : ' ', padding);
    }
    buf_puts(buffer, size, "0b", 2);
    print_number(n, 0, buffer, size, flags, 2, "01");
    if (flags.left_align) {
        buf_repeat_char(buffer, size, ' ', padding);
    }
}

/* %p - вывод указателя */
static void p_handler(va_list *args, char **buffer, size_t *size, format_flags_t flags) {
    void *ptr = va_arg(*args, void*);
    uint32_t n = (uint32_t)(uintptr_t)ptr;
    buf_puts(buffer, size, "0x", 2);
    print_number(n, 0, buffer, size, flags, 16, "0123456789abcdef");
}

/* Вспомогательная функция для вывода состояния пина */
static void print_pin_state(uint8_t state, char **buffer, size_t *size) {
    switch(state) {
        case 0: buf_puts(buffer, size, "LOW", 3); break;
        case 1: buf_puts(buffer, size, "HIGH", 4); break;
        default: buf_puts(buffer, size, "UNKNOWN", 7); break;
    }
}

/* %io - вывод структуры pin_t */
static void io_handler(va_list *args, char **buffer, size_t *size, format_flags_t flags) {
    (void)flags;
    const pin_t *pin = va_arg(*args, const pin_t*);
    
    if (!pin) {
        buf_puts(buffer, size, "(null pin)", 10);
        return;
    }
    
    buf_putc(buffer, size, '{');
    buf_puts(buffer, size, " .name = \"", 10);
    buf_puts(buffer, size, pin->name ? pin->name : "null", -1);
    buf_puts(buffer, size, "\"", 1);
    buf_puts(buffer, size, ", .pin = ", 8);
    
    uint32_t pin_num = pin->pin;
    char tmp[12];
    int i = 0;
    do {
        tmp[i++] = '0' + (pin_num % 10);
        pin_num /= 10;
    } while (pin_num > 0);
    while (i > 0) {
        buf_putc(buffer, size, tmp[--i]);
    }
    
    buf_puts(buffer, size, ", .state = ", 10);
    print_pin_state(pin->state, buffer, size);
    buf_putc(buffer, size, '}');
}
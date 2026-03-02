#include "eprintf.h"
#include "eprintf_formats.h"
#include <string.h>

#define ARRAY_SIZE(arr)         (sizeof(arr) / sizeof((arr)[0]))

#define X(a) void a##_handler(va_list *args, char **buffer, size_t *size, format_flags_t flags);
FORMAT_TABLE
#undef X

#define X(a) {#a, a##_handler},
static const format_extension_t FORMATS[] =
{
  FORMAT_TABLE
};
#undef X

/* Внутренние функции работы с буфером */
void buf_putc(char **buffer, size_t *size, char c) {
    if (*size > 1) {
        **buffer = c;
        (*buffer)++;
        (*size)--;
    }
}

void buf_puts(char **buffer, size_t *size, const char *str, int len) {
    if (!str) str = "(null)";
    if (len < 0) len = (int)strlen(str);
    
    for (int i = 0; i < len && *size > 1; i++) {
        buf_putc(buffer, size, str[i]);
    }
}

void buf_repeat_char(char **buffer, size_t *size, char c, int count) {
    for (int i = 0; i < count && *size > 1; i++) {
        buf_putc(buffer, size, c);
    }
}

/* Парсинг флагов формата */
static const char* parse_format_flags(const char *format, format_flags_t *flags) {
    memset(flags, 0, sizeof(format_flags_t));
    flags->width = -1;
    
    while (*format == '-' || *format == '0') {
        if (*format == '-') {
            flags->left_align = true;
            flags->zero_pad = false;
        } else if (*format == '0' && !flags->left_align) {
            flags->zero_pad = true;
        }
        format++;
    }
    
    if (*format >= '0' && *format <= '9') {
        flags->width = 0;
        while (*format >= '0' && *format <= '9') {
            flags->width = flags->width * 10 + (*format - '0');
            format++;
        }
    }
    
    return format;
}

/* Проверка, может ли символ быть частью спецификатора */
static bool is_spec_char(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

/* Поиск формата в отсортированном массиве */
static int find_format_sorted(const char *format, int *spec_len) {
    for (int i = 0; i < ARRAY_SIZE(FORMATS); i++) {
        int len = (int)strlen(FORMATS[i].spec);
        
        if (strncmp(format, FORMATS[i].spec, len) == 0) {
            char next = format[len];
            if (!is_spec_char(next) || next == '\0') {
                *spec_len = len;
                return i;
            }
        }
    }
    
    *spec_len = 0;
    return -1;
}

/* Обработка спецификатора */
static void process_format(const char **format, va_list *args, 
                            char **buffer, size_t *remaining) {
    (*format)++; // пропускаем '%'
    
    if (**format == '\0') {
        buf_putc(buffer, remaining, '%');
        return;
    }
    
    format_flags_t flags;
    const char *after_flags = parse_format_flags(*format, &flags);
    
    int spec_len;
    int idx = find_format_sorted(after_flags, &spec_len);
    
    if (idx >= 0) {
        FORMATS[idx].handler(args, buffer, remaining, flags);
        *format = after_flags + spec_len;
    } else {
        // Неизвестный формат
        buf_putc(buffer, remaining, '%');
        while (*format < after_flags) {
            buf_putc(buffer, remaining, **format);
            (*format)++;
        }
        if (**format) {
            buf_putc(buffer, remaining, **format);
            (*format)++;
        }
    }
}

/* Основная функция */
int my_vsnprintf(char *str, size_t size, const char *format, va_list args) {
    if (!str || size == 0) return 0;
    
    char *start = str;
    char *buffer = str;
    size_t remaining = size;
    
    while (*format && remaining > 1) {
        if (*format != '%') {
            buf_putc(&buffer, &remaining, *format++);
        } else {
            process_format(&format, &args, &buffer, &remaining);
        }
    }
    
    if (remaining > 0) {
        *buffer = '\0';
    } else if (size > 0) {
        str[size - 1] = '\0';
    }
    
    return (int)(buffer - start);
}

/* Базовый snprintf */
int my_snprintf(char *str, size_t size, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int result = my_vsnprintf(str, size, format, args);
    va_end(args);
    return result;
}
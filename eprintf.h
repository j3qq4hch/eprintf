#ifndef PRINTF_CORE_H
#define PRINTF_CORE_H

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Структура для хранения флагов форматирования */
typedef struct {
    bool left_align;      // Флаг выравнивания влево ('-')
    bool zero_pad;        // Флаг заполнения нулями ('0')
    int width;            // Минимальная ширина поля
} format_flags_t;

/* Тип функции-обработчика */
typedef void (*format_handler_t)(va_list *args, char **buffer, 
                                  size_t *size, format_flags_t flags);

/* Структура для регистрации форматов */
typedef struct {
    const char *spec;
    format_handler_t handler;
} format_extension_t;

/* Функции для работы с буфером */
void buf_putc(char **buffer, size_t *size, char c);
void buf_puts(char **buffer, size_t *size, const char *str, int len);
void buf_repeat_char(char **buffer, size_t *size, char c, int count);

/* Основные функции форматирования */
int my_vsnprintf(char *str, size_t size, const char *format, va_list args);
int my_snprintf(char *str, size_t size, const char *format, ...);

#endif /* PRINTF_CORE_H */
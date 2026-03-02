#include <string.h>
#include <stdint.h>
#include "eprintf.h"
static const char *digits  = "0123456789ABCEF";

/* Вывод числа без внутреннего буфера */
static void print_number(uint32_t n, int sign, char **buffer, size_t *size,
                         format_flags_t flags, int base) {
    // 1. Подсчёт количества цифр
    int num_digits = 1;
    uint32_t temp = n;
    while (temp >= (uint32_t)base) {
        num_digits++;
        temp /= base;
    }

    // 2. Вычисление максимальной степени base (base^(num_digits-1))
    uint32_t power = 1;
    for (int i = 1; i < num_digits; i++) {
        power *= base;
    }

    // 3. Обработка знака и выравнивания
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

    // 4. Вывод цифр от старшей к младшей
    for (int i = 0; i < num_digits; i++) {
        uint8_t digit = n / power;          // текущая цифра
        buf_putc(buffer, size, digits[digit]);
        n %= power;                          // убираем старшую цифру
        power /= base;                        // уменьшаем степень
    }

    // 5. Заполнение справа при выравнивании влево
    if (flags.left_align && padding > 0) {
        buf_repeat_char(buffer, size, ' ', padding);
    }
}

void s_handler(va_list *args, char **buffer, size_t *size, format_flags_t flags) 
{    
    const char *str = va_arg(*args, const char*);
    if (!str) str = "(null)";
    
    int len = (int)strlen(str);
    int padding = (flags.width > len) ? flags.width - len : 0;
    
    if (!flags.left_align && padding > 0) 
    {
        buf_repeat_char(buffer, size, ' ', padding);
    }
    buf_puts(buffer, size, str, len);
    if (flags.left_align && padding > 0) 
    {
        buf_repeat_char(buffer, size, ' ', padding);
    }
}

void c_handler(va_list *args, char **buffer, size_t *size, format_flags_t flags)
{
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

void i_handler(va_list *args, char **buffer, size_t *size, format_flags_t flags) 
{
    int32_t val = va_arg(*args, int32_t);
    int sign = (val < 0) ? -1 : 0;
    uint32_t abs_val = (val < 0) ? (uint32_t)(-val) : (uint32_t)val;
    print_number(abs_val, sign, buffer, size, flags, 10);
}

void x_handler(va_list *args, char **buffer, size_t *size, format_flags_t flags) 
{  
    uint32_t val = va_arg(*args, uint32_t);
    print_number(val, 0, buffer, size, flags, 16);
}

void b_handler(va_list *args, char **buffer, size_t *size, format_flags_t flags) 
{
    uint32_t val = va_arg(*args, uint32_t);
    print_number(val, 0, buffer, size, flags, 2);
}


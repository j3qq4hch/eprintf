#ifndef PRINTF_EXTENSIONS_H
#define PRINTF_EXTENSIONS_H

#include "printf_core.h"

/* Определение структуры pin_t */
typedef struct {
    const char *name;
    uint32_t pin;
    uint8_t state;
} pin_t;

/* Массив форматов - объявление */
extern const format_extension_t FORMATS[];
extern const int FORMATS_COUNT;

#endif /* PRINTF_EXTENSIONS_H */
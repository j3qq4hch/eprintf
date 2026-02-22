# Eprintf (ebucci printf)

Eprintf is an alternative formatted output functions intended to be used with embedded systems with **LIMITED RESOURCES**.

`Eprintf` does not offer the support of all the format specifiers that original `printf` supports. Instead it provides 5 basic formats: `%s`, `%c`, `%i`, `%x`, `%b` and a convenient way of adding more even custom specifiers. 
It is also small, fast, thread-safe and completely static.

`Eprintf` is delivered as a single header-only STB-like library. In order to use it macro `EPRINTF_IMPLEMENTATION` must be defined in one (and only one) place of the project before `#include eprint.fh` statement.
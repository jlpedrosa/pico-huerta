#include <stdarg.h>
#include <stdio.h>

#include "pico/stdlib.h"



int log(const char * str, ... ) {
   va_list arg;
   va_start(arg, str);
   int nChars = vprintf(str, arg);
   va_end(arg);
   return nChars;
}
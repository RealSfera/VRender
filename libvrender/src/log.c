
#include "log.h"
#include <stdio.h>
#include <stdarg.h>

static FILE *log_file = NULL;

int log_init()
{
    log_file = fopen("vrender.log", "w");

    if(!log_file) {
        return 0;
    }

    return 1;
}

void log_printf(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);

    vfprintf(log_file, format, ap);
    fflush(log_file);

    vprintf(format, ap);

    va_end(ap);
}

void log_close()
{
    fclose(log_file);
}

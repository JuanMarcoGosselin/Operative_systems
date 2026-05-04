#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include "utils.h"

// Logging
void log_info(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    printf("[%02d:%02d:%02d] [INFO] ",
        t->tm_hour, t->tm_min, t->tm_sec);

    vprintf(fmt, args);
    printf("\n");

    va_end(args);
}

void log_error(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    printf("[%02d:%02d:%02d] [ERROR] ",
        t->tm_hour, t->tm_min, t->tm_sec);

    vprintf(fmt, args);
    printf("\n");

    va_end(args);
}


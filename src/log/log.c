#include "log.h"

#include "common/error.h"
#include "common/version.h"
#include "system/systick.h"

#include <pico/stdlib.h>

#include <stdio.h>

const char *get_log_level_name(enum log_level level)
{
    switch (level) {
    case LOG_LEVEL_CRITICAL:
        return "CRITICAL";
    case LOG_LEVEL_ERROR:
        return "ERROR";
    case LOG_LEVEL_WARNING:
        return "WARNING";
    case LOG_LEVEL_INFO:
        return "INFO";
    case LOG_LEVEL_DEBUG:
        return "DEBUG";
    case LOG_LEVEL_TRACE:
        return "TRACE";
    default:
        return "NONE";
    }
}

void log_write(const char *module, enum log_level level, const char *func, int line,
               const char *msg)
{
    printf("%lu [%s] - (%s %s:%i): %s\r\n", systick_now_ms(), get_log_level_name(level), module,
           func, line, msg);
}

const char *get_error_name(enum error err)
{
    switch (err) {
    case E_SUCCESS:
        return "SUCCESS";
    case E_INVALID_INPUT:
        return "INVALID_INPUT";
    case E_NULL_POINTER:
        return "NULL_POINTER";
    case E_BUFFER_FULL:
        return "BUFFER_FULL";
    case E_BUFFER_SIZE:
        return "BUFFER_SIZE";
    case E_HARDWARE:
        return "HARDWARE";
    case E_NOT_IMPLEMENTED:
        return "NOT_IMPLEMENTED";
    case E_IO:
        return "IO";
    case E_INVALID_STATE:
        return "INVALID_STATE";
    case E_TIMEOUT:
        return "TIMEOUT";
    case E_GENERIC:
        return "GENERIC";
    default:
        return "NONE";
    }
}

void log_error(const char *module, enum log_level level, const char *func, int line, enum error err,
               const char *msg)
{
    printf("%lu [%s] - (%s %s:%i): ERROR_%s. %s\r\n", systick_now_ms(), get_log_level_name(level),
           module, func, line, get_error_name(err), msg);
}

void log_open(void *callback(const char *))
{
    (void)callback;

    stdio_init_all();
    printf("%lu [%s] - DivaCon Gold v%u.%u.%u\r\n", systick_now_ms(),
           get_log_level_name(LOG_LEVEL_INFO), version.major, version.minor, version.patch);
}

void log_close()
{
}

void log_subscribe(const char *module, enum log_level level)
{
    (void)module;
    (void)level;
}

void log_unsubscribe(const char *module)
{
    (void)module;
}

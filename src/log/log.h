#ifndef LOG_H_
#define LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common/error.h"

enum log_level {
    LOG_LEVEL_CRITICAL,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_TRACE,
};

void log_open(void *callback(const char *));

void log_write(const char *module, enum log_level level, const char *func, int line,
               const char *msg);

void log_error(const char *module, enum log_level level, const char *func, int line, enum error err,
               const char *msg);

/*
 * #define LOGGING_MODULE_NAME "module" at the top of the C file in order to use
 * the log helper macros
 */
#define LOG(level, msg) log_write(LOGGING_MODULE_NAME, level, __func__, __LINE__, msg)

#define LOG_ERROR(level, err, msg)                                                                 \
    log_error(LOGGING_MODULE_NAME, level, __func__, __LINE__, err, msg)

#ifdef __cplusplus
}
#endif

#endif // LOG_H_

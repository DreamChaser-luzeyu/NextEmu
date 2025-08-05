#pragma once

#include <stdio.h>
#include <string.h>

#include "sdkconfig.h"

#if CONFIG_CONSOLE_THREAD_SAFETY
#include "sdk/global_mutex.h"
#include <mutex>
#define STDOUT_ACQUIRE_LOCK do { Builtin_ns::STDOutMutex_GV.lock(); } while(0)
#define STDOUT_RELEASE_LOCK do { Builtin_ns::STDOutMutex_GV.unlock(); } while(0)
#else
#define STDOUT_ACQUIRE_LOCK do {} while(0)
#define STDOUT_RELEASE_LOCK do {} while(0)
#endif

const static char* STYLE_RST         = "\033[0m";
// ----- Background Color
const static char* STYLE_BKG_RED     = "\033[41m";
const static char* STYLE_BKG_GREEN   = "\033[42m";
const static char* STYLE_BKG_YELLOW  = "\033[43m";
// ----- Text Color
const static char* STYLE_TEXT_RED    = "\033[91m";
const static char* STYLE_TEXT_GREEN  = "\033[92m";
const static char* STYLE_TEXT_YELLOW = "\033[33m";
const static char* STYLE_TEXT_BLUE   = "\033[34m";
// ----- Log Color
const static char* STYLE_ERR   = STYLE_TEXT_RED;
const static char* STYLE_WARN  = STYLE_TEXT_YELLOW;
const static char* STYLE_INFO  = STYLE_TEXT_BLUE;
const static char* STYLE_DEBUG = "";
// ----- Control
const static char* CTRL_CLEAR  = "\033[2J";

#define LOG_ERRNO(err_desc) do {                        \
    STDOUT_ACQUIRE_LOCK;                                \
    printf("%s", STYLE_ERR);                            \
    printf("[ERROR] ");                                 \
    printf("%s : ", err_desc);                          \
    printf("%s(errno: %d)", strerror(errno), errno);    \
    printf("%s\n", STYLE_RST);                          \
    STDOUT_RELEASE_LOCK;                                \
} while(0)

#define LOG_ERRNO_AND_EXIT(err_desc) do { \
    LOG_ERRNO(err_desc);                  \
    exit(EXIT_FAILURE);                   \
} while(0)

#define LOG_ERR(...) do {             \
    STDOUT_ACQUIRE_LOCK;              \
    printf("%s", STYLE_ERR);          \
    printf("[ERROR] ");               \
    printf(__VA_ARGS__);              \
    printf("%s\n", STYLE_RST);        \
    STDOUT_RELEASE_LOCK;              \
} while(0)

#define LOG_WARN(...) do {            \
    STDOUT_ACQUIRE_LOCK;              \
    printf("%s", STYLE_WARN);         \
    printf("[WARN ] ");               \
    printf(__VA_ARGS__);              \
    printf("%s\n", STYLE_RST);        \
    STDOUT_RELEASE_LOCK;              \
} while(0)

#define LOG_INFO(...) do {            \
    STDOUT_ACQUIRE_LOCK;              \
    printf("%s", STYLE_INFO);         \
    printf("[INFO ] ");               \
    printf(__VA_ARGS__);              \
    printf("%s\n", STYLE_RST);        \
    STDOUT_RELEASE_LOCK;              \
} while(0)

#if CONFIG_DEBUG_ENABLE
#define LOG_DEBUG(...) do {       \
    STDOUT_ACQUIRE_LOCK;          \
    printf("%s", STYLE_DEBUG);    \
    printf("[DEBUG] ");           \
    printf(__VA_ARGS__);          \
    printf("%s\n", STYLE_RST);    \
    STDOUT_RELEASE_LOCK;          \
} while(0)
#else
#define LOG_DEBUG(...)
#endif

#if CONFIG_DEBUG_ENABLE
#define LOG_CLEAR()
#else
#define LOG_CLEAR() do {              \
    STDOUT_ACQUIRE_LOCK;              \
    printf("%s", CTRL_CLEAR);         \
    STDOUT_RELEASE_LOCK;              \
} while(0)
#endif




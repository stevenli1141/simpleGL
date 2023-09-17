#ifndef GUARD_LOG_H
#define GUARD_LOG_H

#include <stdio.h>
#include "constants/vars.h"
#include "config.h"
#include "system.h"

#ifdef DEBUG_VERBOSE
#define LOG_INFO(...) printf("T%06d [INFO] ", g_system_vars[SYSVAR_FRAMES]); printf(__VA_ARGS__);
#else
#define LOG_INFO(...) while(0) { }
#endif

#ifdef DEBUG_WARNINGS
#define LOG_WARN(...) printf("T%06d [WARN] ", g_system_vars[SYSVAR_FRAMES]); printf(__VA_ARGS__);
#else
#define LOG_WARN(...) while(0) { }
#endif

#ifdef DEBUG_ERRORS
#define LOG_ERROR(...) printf("T%06d [ERROR] ", g_system_vars[SYSVAR_FRAMES]); printf(__VA_ARGS__);
#else
#define LOG_ERROR(...) while(0) { }
#endif

#endif

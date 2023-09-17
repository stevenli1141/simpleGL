#ifndef GUARD_SYSTEM_H
#define GUARD_SYSTEM_H

#include "types.h"

#define EXIT_VALUE_NONE 0
#define EXIT_VALUE_QUIT 1

extern void main_cycle(void);
extern const u8 run_periodic_worker(void);

extern void start_system(void);

// System vars 0-127 are free for scripts to use for anything that context registers can't be used for e.g. global information
// System vars 128-255 are meant to be read-only by scripts and hold values determined by other systems e.g. SYSVAR_CLOCK
extern s32 g_system_vars[];

extern u64 g_start_time;

#endif

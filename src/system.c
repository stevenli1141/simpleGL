#include <time.h>
#include "constants/vars.h"
#include "controller.h"
#include "log.h"
#include "system.h"

// This starts all other workers and so is guaranteed to run every cycle

u64 g_start_time;
s32 g_system_vars[256];

const u8 run_periodic_worker(void) {
    // Run the controller input update at the start
    controller_input_update();
    if (g_controller_quit) {
        return EXIT_VALUE_QUIT;
    }

    // update clock. this could potentially loop around to negative (if left running for 68 years), so keep this extreme case in mind
    g_system_vars[SYSVAR_CLOCK] = time(NULL) - g_start_time;
    g_system_vars[SYSVAR_FRAMES]++;

    return EXIT_VALUE_NONE;
}

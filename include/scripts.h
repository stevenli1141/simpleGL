#ifndef GUARD_SCRIPTS_H
#define GUARD_SCRIPTS_H

#include "types.h"

// Script instructions are like assembly instructions that are executed
// Idea is we can implement script commands, this is a container for script execution contexts.
// Script instructions should be 4-byte aligned data blocks. To implement bigger instructions the handler can further advance the script ptr.

#define NUM_CONTEXT_VARS 16
#define NUM_CONTEXTS 8
#define NO_SCRIPT NUM_CONTEXTS

struct script_context {
    u8 id;
    const u8 *start_ptr;
    const u8 *pc; // can be altered by branching statements etc.
    bool8 suspended;
    bool8 yielded;
    u8 caller;
    s32 vars[16]; // 4 bytes enough for 4 chars, int or float
};

typedef const u8 (*script_cmd_cb)(struct script_context *);


// Execution
extern const u8 start_new_script(const u8 *script_start, const u8 caller, const u8 arg);
extern void halt_script_execution(const u8 context_id);
extern void set_script_suspended(const u8 context_id, const bool8 suspended);

// Testing only
extern void test_print_scripts(void);

// Pointers
extern const u8 g_test_script_1[];
extern const u8 g_test_script_2[];

#endif

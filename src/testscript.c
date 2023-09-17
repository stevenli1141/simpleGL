#include "constants/commands.h"
#include "log.h"
#include "scripts.h"

static const char cmd_names[][24] = {
    "no-op",
    "setvar",
    "SETSYSVAR",
    "loadvar",
    "add",
    "sub",
    "mult",
    "div",
    "mod",
    "cmp",
    "setflag",
    "goto",
    "goto_if_eq",
    "goto_if_set",
    "return",
    "call",
    "waitfor",
    "yield",
    "setlocked",
    "special",
    "loadmap",
    "moveplayer",
    "setplayertransform",
    "setplayervalue",
    "moveentity",
    "setentitytransform",
    "faceplayer"
    "setentityvalue",
    "movecamera",
    "setcameratransform",
    "jump",
    "dialog",
    "simpledialog"
};

static void print_format_script(const u8 *script);

void test_print_scripts(void) {
    LOG_INFO("g_test_script_1:\n")
    print_format_script(g_test_script_1);
    LOG_INFO("g_test_script_2:\n")
    print_format_script(g_test_script_2);
}

static void print_format_script(const u8 *script) {
    for (u8 i = 0; i < 512; i += 4) {
        const u8 cmd = script[i];
        if (cmd < 32) {
            LOG_INFO("%d\t%s %d %d %d\n", i, cmd_names[cmd], script[i+1], script[i+2], script[i+3])
        } else {
            LOG_INFO("%d\t%d %d %d %d\n", i, cmd, script[i+1], script[i+2], script[i+3])
        }
        if (cmd == CMD_RETURN) {
            break;
        }
    }
}

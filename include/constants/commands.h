#ifndef GUARD_COMMANDS_H
#define GUARD_COMMANDS_H

#define NOOP                        0x0
#define CMD_SETVAR                  0x1
#define CMD_SETSYSVAR               0x2
#define CMD_LOADVAR                 0x3
#define CMD_ADD                     0x4
#define CMD_SUB                     0x5
#define CMD_MULT                    0x6
#define CMD_DIV                     0x7
#define CMD_MOD                     0x8
#define CMD_CMP                     0x9
#define CMD_SETFLAG                 0xA
#define CMD_GOTO                    0xB
#define CMD_GOTO_IF_EQ              0xC
#define CMD_GOTO_IF_SET             0xD
#define CMD_RETURN                  0xE
#define CMD_CALL                    0xF
#define CMD_WAIT_FOR                0x10
#define CMD_YIELD                   0x11
#define CMD_LOCK_UNLOCK             0x12
#define CMD_SPECIAL                 0x13
#define CMD_LOAD_MAP                0x14
#define CMD_MOVE_USER_TO            0x15
#define CMD_SET_USER_TRANSFORM      0x16
#define CMD_SET_USER_VALUE          0x17
#define CMD_MOVE_ENTITY_TO          0x18
#define CMD_SET_ENTITY_TRANSFORM    0x19
#define CMD_FACE_USER               0x1A
#define CMD_SET_ENTITY_VALUE        0x1B
#define CMD_MOVE_CAMERA_TO          0x1C
#define CMD_SET_CAMERA_TRANSFORM    0x1D
#define CMD_FADE_TO                 0x21
#define CMD_PREPARE_TEXT            0x25
#define CMD_PREPARED_SHARED_TEXT    0x26
#define CMD_SET_DYNAMIC_SUBOBJ      0x2A
#define CMD_MOVE_DYNAMIC_SUBOBJ_TO  0x2B
#define CMD_CLOSE_DIALOG            0x30
#define CMD_RANDOM                  0x31
#define CMD_ADD_DYNAMIC_OBJ         0x5C
#define CMD_MOVE_DYNAMIC_OBJ        0x5D
#define CMD_CHANGE_DYNAMIC_OBJ_RGB  0x5E
#define CMD_REMOVE_DYNAMIC_OBJ      0x5F

#define NUM_COMMANDS                0x80

#endif

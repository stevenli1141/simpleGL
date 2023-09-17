ifeq ($(WINDOWS),1)
CC := x86_64-w64-mingw32-gcc
BUILDNAME := simplegl_windows
else
CC := gcc
BUILDNAME := simplegl
endif
PYTHON = /usr/bin/python3
SHADER_SCRIPT = tools/shader_src.py

OBJ_DIR := build

C_SUBDIR := src
C_DATA_SUBDIR := data/data
SCRIPT_SUBDIR := data/scripts
SCENE_SUBDIR := data/maps
IMG_SUBDIR := data/images
DYNAMIC_OBJ_SUBDIR := data/objects
SHADER_SUBDIR := data/shaders
AUDIO_SUBDIR := data/audio
# MID_SUBDIR = data/midi

C_BUILDDIR := $(OBJ_DIR)/$(C_SUBDIR)
C_DATA_BUILDDIR := $(OBJ_DIR)/$(C_DATA_SUBDIR)
SCRIPT_BUILDDIR := $(OBJ_DIR)/$(SCRIPT_SUBDIR)
SCENE_BUILDDIR := $(OBJ_DIR)/$(SCENE_SUBDIR)
DYNAMIC_OBJ_BUILDDIR := $(OBJ_DIR)/$(DYNAMIC_OBJ_SUBDIR)
IMG_BUILDDIR := $(OBJ_DIR)/$(IMG_SUBDIR)
SHADER_BUILDDIR := $(OBJ_DIR)/$(SHADER_SUBDIR)
AUDIO_BUILDDIR = $(OBJ_DIR)/$(AUDIO_SUBDIR)
# MID_BUILDDIR = $(OBJ_DIR)/$(MID_SUBDIR)

$(shell mkdir -p $(C_BUILDDIR) $(IMG_BUILDDIR) $(AUDIO_BUILDDIR))

C_SRCS := $(shell find $(C_SUBDIR) -name '*.c')
C_OBJS := $(C_SRCS:$(C_SUBDIR)/%.c=$(C_BUILDDIR)/%.o)

C_DATA_SRCS := $(wildcard $(C_DATA_SUBDIR)/*.c)
C_DATA_OBJS := $(patsubst $(C_DATA_SUBDIR)/%.c,$(C_DATA_SUBDIR)/%.o,$(C_DATA_SRCS))

SCRIPT_SRCS := $(wildcard $(SCRIPT_SUBDIR)/*.s)
SCRIPT_OBJS := $(patsubst $(SCRIPT_SUBDIR)/%.s,$(SCRIPT_BUILDDIR)/%.o,$(SCRIPT_SRCS))

SCENE_SRCS := $(wildcard $(SCENE_SUBDIR)/*/*.c)
SCENE_OBJS := $(patsubst $(SCENE_SUBDIR)/%.c,$(SCENE_BUILDDIR)/%.o,$(SCENE_SRCS))

SCENE_ASM_SRCS := $(wildcard $(SCENE_SUBDIR)/*/*.s)
SCENE_ASM_OBJS := $(patsubst $(SCENE_SUBDIR)/%.s,$(SCENE_BUILDDIR)/%.o,$(SCENE_ASM_SRCS))

DYNAMIC_OBJ_SRCS := $(wildcard $(DYNAMIC_OBJ_SUBDIR)/*.c)
DYNAMIC_OBJ_OBJS := $(patsubst $(DYNAMIC_OBJ_SUBDIR)/%.c,$(DYNAMIC_OBJ_BUILDDIR)/%.o,$(DYNAMIC_OBJ_SRCS))

IMG_SRCS := $(wildcard $(IMG_SUBDIR)/*.png $(IMG_SUBDIR)/*/*.png)
IMG_BINS := $(patsubst $(IMG_SUBDIR)/%.png,$(IMG_SUBDIR)/%.rgba,$(IMG_SRCS))

IMG_INCS := $(wildcard $(IMG_SUBDIR)/*.s)
IMG_OBJS := $(patsubst $(IMG_SUBDIR)/%.s,$(IMG_BUILDDIR)/%.o,$(IMG_INCS))

SHADER_SRCS := $(wildcard $(SHADER_SUBDIR)/*)
SHADER_OBJS := $(patsubst $(SHADER_SUBDIR)/%.glsl,$(SHADER_BUILDDIR)/%.o,$(SHADER_SRCS))

# AUDIO_SRCS := $(wildcard $(AUDIO_SUBDIR)/*.s)
# AUDIO_OBJS := $(patsubst $(AUDIO_SUBDIR)/%.s,$(AUDIO_BUILDDIR)/%.o,$(AUDIO_SRCS))

# MID_SRCS := $(wildcard $(MID_SUBDIR)/*.mid)
# MID_OBJS := $(patsubst $(MID_SUBDIR)/%.mid,$(MID_BUILDDIR)/%.o,$(MID_SRCS))

DEPS := $(IMG_BINS)
OBJS := $(C_OBJS) $(C_DATA_OBJS) $(SCRIPT_OBJS) $(SCENE_OBJS) $(SCENE_ASM_OBJS) $(DYNAMIC_OBJ_OBJS) $(IMG_OBJS) $(SHADER_OBJS)

all: $(OBJ_DIR)/$(BUILDNAME)

%.rgba: %.png
	convert $< $@

%.c: %.glsl
	$(PYTHON) $(SHADER_SCRIPT) $< > $@

INC_DIRS := $(shell find include -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
ifeq ($(WINDOWS),1)
CFLAGS := $(INC_FLAGS) -Ilib/SDL2-2.0.22/x86_64-w64-mingw32/include/SDL2 -Ilib/SDL2_mixer-2.0.4/x86_64-w64-mingw32/include/SDL2 -Ilib/GLEW/mingw64/include
LDFLAGS := -Llib/SDL2-2.0.22/x86_64-w64-mingw32/bin -Llib/SDL2_mixer-2.0.4/x86_64-w64-mingw32/bin -lSDL2 -lSDL2_mixer -llibogg-0 -llibvorbis-0 -llibvorbisfile-3 -Llib/GLEW/mingw64/bin -lglew32 -lopengl32
else
CFLAGS := $(INC_FLAGS)
LDFLAGS := -lSDL2 -lSDL2_image
endif

$(SCRIPT_BUILDDIR)/%.o: $(SCRIPT_SUBDIR)/%.s
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(SCENE_BUILDDIR)/%.o: $(SCENE_SUBDIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(SCENE_BUILDDIR)/%.o: $(SCENE_SUBDIR)/%.s
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(DYNAMIC_OBJ_BUILDDIR)/%.o: $(DYNAMIC_OBJ_SUBDIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(IMG_BUILDDIR)/%.o: $(IMG_SUBDIR)/%.s
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(SHADER_BUILDDIR)/%.o: $(SHADER_SUBDIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(C_DATA_BUILDDIR)/%.o: $(C_DATA_SUBDIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(C_BUILDDIR)/%.o: $(C_SUBDIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/$(BUILDNAME): $(DEPS) $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

.PHONY: all windows clean
cleanimg:
	rm $(OBJ_DIR)/$(IMG_SUBDIR)/images.o
cleansrc:
	rm -rf $(OBJ_DIR)/$(C_SUBDIR)
mostlyclean:
	rm -rf $(OBJ_DIR)/$(C_SUBDIR)
	rm -rf $(OBJ_DIR)/$(SCRIPT_SUBDIR)
	rm -rf $(OBJ_DIR)/$(SCENE_SUBDIR)
clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(IMG_SUBDIR)/*.rgba

windows: ; @$(MAKE) WINDOWS=1

# -include $(DEPS)

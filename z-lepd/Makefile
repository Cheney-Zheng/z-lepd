PROJECT_TOP_DIR=$(PWD)
PROJECT_OBJ_DIR=$(PROJECT_TOP_DIR)/.objs
PROJECT_LIB_DIR=$(PROJECT_TOP_DIR)/libs
PROJECT_EV_DIR=$(PROJECT_LIB_DIR)/arm-libev
PROJECT_PREBUILT_DIR=$(PROJECT_TOP_DIR)/prebuilt-binaries

PROJECT_INC_DIR=$(PROJECT_TOP_DIR)/include
PROJECT_SRC_DIR=$(PROJECT_TOP_DIR)/src
PROJECT_MODULE_DIR=$(PROJECT_SRC_DIR)/modules
MKDIR := mkdir -p

ARCH ?= x86
ifeq ($(ARCH), x86)
CC=gcc
AR=ar
LD=ld
CFLAGS := -lev -lm -lrt -I$(PROJECT_INC_DIR) -D_BUILTIN_FUNC
LDFLAG :=
else
CROSS_COMPILE=arm-hisiv600-linux-
CC=$(CROSS_COMPILE)gcc
AR=$(CROSS_COMPILE)ar
LD=$(CROSS_COMPILE)ld
#CFLAGS := -lev -lm -lrt -static -I$(PROJECT_INC_DIR) -D_BUILTIN_FUNC
CFLAGS := -lev -lm -lrt -I$(PROJECT_INC_DIR) -D_BUILTIN_FUNC
LDFLAG := -L$(PROJECT_EV_DIR)
endif

export CROSS_COMPILE CC AR LD
#DEFS = -DBUILDIN_FUNC

TARGETS = lepd

src :=$(wildcard $(PROJECT_SRC_DIR)/*.c)
dir := $(notdir $(src))
PROJECT_OBJ := $(patsubst %.c,%.o,$(dir) )

PROJECT_ALL_OBJS := $(addprefix $(PROJECT_OBJ_DIR)/, $(PROJECT_OBJ))

all:$(PROJECT_ALL_OBJS)
	$(CC) $(wildcard $(PROJECT_SRC_DIR)/*.c) $(wildcard $(PROJECT_LIB_DIR)/*.a) $(CFLAGS) -o $(TARGETS) $(LDFLAG)

prepare:
	$(MKDIR) $(PROJECT_OBJ_DIR)
	$(MKDIR) $(PROJECT_LIB_DIR)
 
$(PROJECT_OBJ_DIR)/%.o : $(PROJECT_SRC_DIR)/%.c prepare 
	$(CC) -c $(CFLAGS) $< -o $@ 

clean:
	rm -fr $(PROJECT_OBJ_DIR)
	rm -fr $(TARGETS)
	-rm $(PROJECT_LIB_DIR)/*

install:
ifeq ($(ARCH), x86)
	cp $(TARGETS) $(PROJECT_PREBUILT_DIR)/x86_lepd
else
	cp $(TARGETS) $(PROJECT_PREBUILT_DIR)/arm_lepd
endif

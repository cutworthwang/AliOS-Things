ifneq ($(filter $(HOST_ARCH), rockchiplinux),)

TOOLCHAIN_PATH ?=
TOOLCHAIN_PREFIX := arm-rockchip-linux-gnueabihf-

TOOLCHAIN_DEFAULT_FOLDER := usr

ifneq (,$(wildcard $(COMPILER_ROOT)/$(TOOLCHAIN_DEFAULT_FOLDER)/bin))
TOOLCHAIN_PATH := $(COMPILER_ROOT)/$(TOOLCHAIN_DEFAULT_FOLDER)/bin/
endif

SYSTEM_TOOLCHAIN_PATH :=
ifneq (,$(filter $(HOST_OS),Linux32 Linux64 OSX))
SYSTEM_GCC_PATH = $(shell which $(TOOLCHAIN_PREFIX)gcc)
ifneq (,$(findstring $(TOOLCHAIN_PREFIX)gcc,$(SYSTEM_GCC_PATH)))
SYSTEM_TOOLCHAIN_PATH := $(subst $(TOOLCHAIN_PREFIX)gcc,,$(SYSTEM_GCC_PATH))
endif
else #Linux32 Linux64 OSX
$(error unsupport OS $(HOST_OS))
endif #Linux32 Linux64 OSX

ifeq (,$(TOOLCHAIN_PATH))
ifneq (,$(SYSTEM_TOOLCHAIN_PATH))
TOOLCHAIN_PATH := $(SYSTEM_TOOLCHAIN_PATH)
else
$(error can not find compiler toolchain, please install gcc-arm-linux-gnueabihf toolchain first)
endif #SYSTEM_TOOLCHAIN_PATH
endif #TOOLCHAIN_PATH

CC      := $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)gcc
CXX     := $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)g++
AS      := $(CC)
AR      := $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)ar
LD      := $(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)ld
OPTIONS_IN_FILE_OPTION    := @

ADD_COMPILER_SPECIFIC_STANDARD_CFLAGS   = $(1) $(if $(filter yes,$(MXCHIP_INTERNAL) $(TESTER)),-Werror)
ADD_COMPILER_SPECIFIC_STANDARD_CXXFLAGS = $(1) $(if $(filter yes,$(MXCHIP_INTERNAL) $(TESTER)),-Werror)
ADD_COMPILER_SPECIFIC_STANDARD_ADMFLAGS = $(1)
COMPILER_SPECIFIC_OPTIMIZED_CFLAGS    := -Os
COMPILER_SPECIFIC_UNOPTIMIZED_CFLAGS  := -O0
COMPILER_SPECIFIC_PEDANTIC_CFLAGS  := $(COMPILER_SPECIFIC_STANDARD_CFLAGS)
COMPILER_SPECIFIC_ARFLAGS_CREATE   := -rcs
COMPILER_SPECIFIC_ARFLAGS_ADD      := -rcs
COMPILER_SPECIFIC_ARFLAGS_VERBOSE  := -v

#debug: no optimize and log enable
COMPILER_SPECIFIC_DEBUG_CFLAGS     := -DDEBUG -ggdb $(COMPILER_SPECIFIC_UNOPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_DEBUG_CXXFLAGS   := -DDEBUG -ggdb $(COMPILER_SPECIFIC_UNOPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_DEBUG_ASFLAGS    := -DDEBUG=1
COMPILER_SPECIFIC_DEBUG_LDFLAGS    := -Wl,--gc-sections -Wl,--cref

#release_log: optimize but log enable
COMPILER_SPECIFIC_RELEASE_LOG_CFLAGS   := -ggdb $(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_RELEASE_LOG_CXXFLAGS := -ggdb $(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_RELEASE_LOG_ASFLAGS  :=
COMPILER_SPECIFIC_RELEASE_LOG_LDFLAGS  := -Wl,--gc-sections -Wl,$(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS) -Wl,--cref

#release: optimize and log disable
COMPILER_SPECIFIC_RELEASE_CFLAGS   := -DNDEBUG $(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_RELEASE_CXXFLAGS := -DNDEBUG $(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS)
COMPILER_SPECIFIC_RELEASE_ASFLAGS  :=
COMPILER_SPECIFIC_RELEASE_LDFLAGS  := -Wl,--gc-sections -Wl,$(COMPILER_SPECIFIC_OPTIMIZED_CFLAGS) -Wl,--cref

COMPILER_SPECIFIC_DEPS_FLAG        := -MD
COMPILER_SPECIFIC_COMP_ONLY_FLAG   := -c
COMPILER_SPECIFIC_LINK_MAP         = -Wl,-Map,$(1)
COMPILER_SPECIFIC_LINK_FILES       = -Wl,--start-group $(1) -Wl,--end-group
COMPILER_SPECIFIC_LINK_SCRIPT_DEFINE_OPTION =
COMPILER_SPECIFIC_LINK_SCRIPT      =
LINKER                             := $(CC)
LINK_SCRIPT_SUFFIX                 := .ld
TOOLCHAIN_NAME := GCC

ENDIAN_CFLAGS_LITTLE   := -mlittle-endian
ENDIAN_CXXFLAGS_LITTLE := -mlittle-endian
ENDIAN_ASMFLAGS_LITTLE :=
ENDIAN_LDFLAGS_LITTLE  := -mlittle-endian
CLIB_LDFLAGS_NANO      := --specs=nano.specs
CLIB_LDFLAGS_NANO_FLOAT:= --specs=nano.specs -u _printf_float

# Chip specific flags for GCC

CPU_CFLAGS     :=
CPU_CXXFLAGS   :=
CPU_ASMFLAGS   :=
CPU_LDFLAGS    :=
CLIB_LDFLAGS_NANO       +=
CLIB_LDFLAGS_NANO_FLOAT +=

# $(1) is map file, $(2) is CSV output file
COMPILER_SPECIFIC_MAPFILE_TO_CSV = $(PYTHON) $(MAPFILE_PARSER) $(1) > $(2)

MAPFILE_PARSER            :=$(MAKEFILES_PATH)/scripts/map_parse_gcc.py

# $(1) is map file, $(2) is CSV output file
COMPILER_SPECIFIC_MAPFILE_DISPLAY_SUMMARY = $(PYTHON) $(MAPFILE_PARSER) $(1)

KILL_OPENOCD_SCRIPT := $(MAKEFILES_PATH)/scripts/kill_openocd.py

KILL_OPENOCD = $(PYTHON) $(KILL_OPENOCD_SCRIPT)

OBJDUMP := "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)objdump$(EXECUTABLE_SUFFIX)"
OBJCOPY := "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)objcopy$(EXECUTABLE_SUFFIX)"
STRIP   := "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)strip$(EXECUTABLE_SUFFIX)"
NM      := "$(TOOLCHAIN_PATH)$(TOOLCHAIN_PREFIX)nm$(EXECUTABLE_SUFFIX)"

STRIP_OUTPUT_PREFIX := -o
OBJCOPY_BIN_FLAGS   := -O binary -R .eh_frame -R .init -R .fini -R .comment -R .ARM.attributes
OBJCOPY_HEX_FLAGS   := -O ihex -R .eh_frame -R .init -R .fini -R .comment -R .ARM.attributes

LINK_OUTPUT_SUFFIX  :=.elf
BIN_OUTPUT_SUFFIX   :=.bin
HEX_OUTPUT_SUFFIX   :=.hex

endif

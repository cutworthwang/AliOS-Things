VPATH   := $(TOP_DIR)/$(MODULE_NAME)
SRCS    ?= $(foreach M,*.c */*.c */*/*.c,$(wildcard $(TOP_DIR)/$(MODULE_NAME)/$(M))) $(wildcard *.c)
OBJS    := $(SRCS:.c=.o)
OBJS    := $(subst $(TOP_DIR)/$(MODULE_NAME)/,,$(OBJS))

ifdef SRCS_PATTERN
PROG_LIST	:= $(foreach M,$(SRCS_PATTERN),$(shell ls $(TOP_DIR)/$(MODULE_NAME)/$(M) 2>/dev/null))
SRCS 		:= $(PROG_LIST)
OBJS 		:= $(PROG_LIST:.c=.o)
OBJS 		:= $(subst $(TOP_DIR)/$(MODULE_NAME)/,,$(OBJS))
endif

ifdef TARGET

ifneq (modinfo,$(MAKECMDGOALS))
ifneq (clean,$(MAKECMDGOALS))
sinclude $(OBJS:.o=.d)
endif
endif

ifdef LIBA_TARGET
$(TARGET): $(LIBA_TARGET)

LDFLAGS := -l$(subst .a,,$(subst lib,,$(LIBA_TARGET))) $(LDFLAGS)
endif

ifdef LIBSO_TARGET
$(TARGET): $(LIBSO_TARGET)

LDFLAGS := -l$(subst .so,,$(subst lib,,$(LIBSO_TARGET))) $(LDFLAGS)
endif

LDFLAGS += $(sort $(CONFIG_ENV_LDFLAGS))

ifneq (,$(filter %.cpp %.cc,$(SRCS)))
CCLD    := $(CXX)
else
CCLD    := $(CC)
endif

ifeq (1,$(words $(TARGET)))

$(TARGET): $(OBJS) FORCE
	$(call Inspect_Env,$(WATCHED_VARS))
	$(Q) \
( \
	if [ "$(strip $(CC))" = "gcc" ] || [ "$(filter -D_PLATFORM_IS_LINUX_,$(CFLAGS))" != "" ]; then \
	    $(call Brief_Log,"LD"); \
	    mkdir -p $(OUTPUT_DIR)${bindir}; \
	    $(CCLD) $(CFLAGS) -o $@ \
	        $(RPATH_CFLAGS) \
	        $(OBJS) \
	        -Wl,--start-group $(filter -l%, $(LDFLAGS)) -Wl,--end-group \
	        $(filter -L%, $(LDFLAGS)) && \
	    cp -f $@ $(OUTPUT_DIR)${bindir}; \
	fi; \
)

else

$(foreach t,$(sort $(TARGET)),$(t)): FORCE
	$(Q)$(MAKE) TARGET=$@ OBJS="$(SRCS_$@:.c=.o)"

endif

endif   # ifdef TARGET

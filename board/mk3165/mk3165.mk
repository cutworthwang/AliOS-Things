NAME := board_mk3165

JTAG := jlink_swd

$(NAME)_TYPE := kernel
MODULE               := 3165
HOST_ARCH            := Cortex-M4
HOST_MCU_FAMILY      := stm32f4xx
HOST_MCU_VARIANT     := STM32F411
SUPPORT_BINS         := no

BUS := SDIO

$(NAME)_SOURCES := board.c mico_spi.c
$(NAME)_PREBUILT_LIBRARY := MiCO.$(MODULE).$(TOOLCHAIN_NAME).a

GLOBAL_INCLUDES += ./
GLOBAL_DEFINES += HSE_VALUE=26000000

GLOBAL_DEFINES += STDIO_UART=0 
GLOBAL_DEFINES += RHINO_CONFIG_TICK_TASK=0 RHINO_CONFIG_WORKQUEUE=0

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_MK3165
CONFIG_SYSINFO_DEVICE_NAME := MK3165

GLOBAL_CFLAGS += -DSYSINFO_OS_VERSION=\"$(CONFIG_SYSINFO_OS_VERSION)\"
GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

GLOBAL_LDFLAGS  += -L $(SOURCE_ROOT)/board/mk3165

# Global defines
# HSE_VALUE = STM32 crystal frequency = 26MHz (needed to make UART work correctly)
GLOBAL_DEFINES += $$(if $$(NO_CRLF_STDIO_REPLACEMENT),,CRLF_STDIO_REPLACEMENT)
GLOBAL_CFLAGS  += -DSTM32F411 -mcpu=cortex-m4 -mthumb -mfloat-abi=soft

WIFI_FIRMWARE_SECTOR_START    := 2      #0x2000
FILESYSTEM_IMAGE_SECTOR_START := 256    #0x100000

# Extra build target in mico_standard_targets.mk, include bootloader, and copy output file to eclipse debug file (copy_output_for_eclipse)
EXTRA_TARGET_MAKEFILES +=  $(MAKEFILES_PATH)/aos_standard_targets.mk
#EXTRA_TARGET_MAKEFILES +=  $(SOURCE_ROOT)/platform/mcu/$(HOST_MCU_FAMILY)/gen_crc_bin.mk

# ==============================================================================
# GPIO Driver - STM32 Example Makefile
# Requires: arm-none-eabi-gcc and libopencm3
# ==============================================================================

# Compiler
CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

# Project Name
PROJECT = index

# Directories
ROOT_DIR = ../..
LOGGING_DIR = $(ROOT_DIR)/../logging_driver
BUILD_DIR = build

# Flags
CFLAGS = -Wall -Wextra -std=c99 -Os -g3 \
         -mcpu=cortex-m3 -mthumb -msoft-float \
         -DPLATFORM_STM32 \
         -DSTM32F1

# Includes
INCLUDES = -I$(ROOT_DIR) \
           -I$(LOGGING_DIR) \
           -I../common \
           -I$(ROOT_DIR)/../helper_utils \
           -I/usr/local/include  # Adjust for libopencm3 headers

# Sources
SRCS = $(ROOT_DIR)/config/gpio_config.c \
       $(ROOT_DIR)/gpioLib.c \
       $(ROOT_DIR)/implementations/stm32/gpioPlatform_stm32.c \
       $(LOGGING_DIR)/logLib.c \
       $(LOGGING_DIR)/implementations/logPlatform_console.c \
       ../common/app_main.c \
       ../../implementations/stm32/platform_adapter.c \
       $(ROOT_DIR)/helpers/gpio_helper.c

# Object Files
OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(SRCS)))

# Linker Script (Update to match your board)
LDSCRIPT = stm32f103c8t6.ld 
LDFLAGS = -T$(LDSCRIPT) -L/usr/local/lib -lopencm3_stm32f1 --static -nostartfiles -Wl,--gc-sections

.PHONY: all clean prepare flash

all: prepare $(BUILD_DIR)/$(PROJECT).elf $(BUILD_DIR)/$(PROJECT).bin

prepare:
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)

# Compilation Rules
$(BUILD_DIR)/gpio_config.o: $(ROOT_DIR)/config/gpio_config.c | prepare
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/gpioLib.o: $(ROOT_DIR)/gpioLib.c | prepare
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/gpioPlatform_stm32.o: $(ROOT_DIR)/implementations/stm32/gpioPlatform_stm32.c | prepare
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/platform_adapter.o: ../../implementations/stm32/platform_adapter.c | prepare
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/logLib.o: $(LOGGING_DIR)/logLib.c | prepare
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/logPlatform_console.o: $(LOGGING_DIR)/implementations/logPlatform_console.c | prepare
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/app_main.o: ../common/app_main.c | prepare
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/gpio_helper.o: $(ROOT_DIR)/helpers/gpio_helper.c | prepare
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Linking
$(BUILD_DIR)/$(PROJECT).elf: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/$(PROJECT).bin: $(BUILD_DIR)/$(PROJECT).elf
	$(OBJCOPY) -O binary $< $@
	@echo.
	@echo ========================================
	@echo Build successful: $@
	@echo ========================================

clean:
	@if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)

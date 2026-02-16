# ==============================================================================
# AVR Makefile for GPIO Driver Example
# Target: ATmega328P (Arduino Uno standard)
# ==============================================================================

# Microcontroller Settings
MCU = atmega328p
F_CPU = 16000000UL
BAUD = 115200

# Directory Paths
ROOT_DIR = ../..
LOGGING_DIR = $(ROOT_DIR)/../logging_driver
HELPER_DIR = $(ROOT_DIR)/helpers
IMPL_DIR = $(ROOT_DIR)/implementations
COMMON_DIR = ../common

# Tools
CC = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE_EXE = "C:/Users/kuumc/AppData/Local/Arduino15/packages/arduino/tools/avrdude/6.3.0-arduino17/bin/avrdude.exe"
AVRDUDE_CONF = "C:/Users/kuumc/AppData/Local/Arduino15/packages/arduino/tools/avrdude/6.3.0-arduino17/etc/avrdude.conf"

# Check if explicit path exists, else default to PATH
ifneq ("$(wildcard $(AVRDUDE_EXE))","")
    AVRDUDE = $(AVRDUDE_EXE) -C$(AVRDUDE_CONF)
else
    AVRDUDE = avrdude
endif

# Flags
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DPLATFORM_AVR -Os -Wall -Wextra -std=c99
# Include Paths
INC_DIRS = -I$(ROOT_DIR) \
           -I$(ROOT_DIR)/config \
           -I$(LOGGING_DIR) \
           -I$(HELPER_DIR) \
           -I$(COMMON_DIR) \
           -I$(ROOT_DIR)/../helper_utils
CFLAGS += $(INC_DIRS)

# Source Files
SRCS = $(COMMON_DIR)/app_main.c \
       $(IMPL_DIR)/avr/platform_adapter.c \
       $(ROOT_DIR)/gpioLib.c \
       $(BUILD_DIR)/gpio_config_gen.c \
       $(HELPER_DIR)/gpio_helper.c \
       $(IMPL_DIR)/avr/gpioPlatform_avr.c \
       $(LOGGING_DIR)/logLib.c \
       $(LOGGING_DIR)/implementations/logPlatform_console.c

# Generation Rule
$(BUILD_DIR)/gpio_config_gen.c: config.json
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	@echo [CFG] Generating config from JSON...
	@python ../../scripts/gen_config.py config.json $@

# Build Objects
BUILD_DIR = build
OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(SRCS)))

# Targets
TARGET = $(BUILD_DIR)/gpio_example
HEX = $(TARGET).hex

# VPATH for finding sources
VPATH = $(COMMON_DIR) \
        $(ROOT_DIR) \
        $(ROOT_DIR)/config \
        $(HELPER_DIR) \
        $(IMPL_DIR)/avr \
        $(LOGGING_DIR) \
        $(LOGGING_DIR)/implementations

# Rules
all: prepare $(HEX)

prepare:
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)

$(HEX): $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	@echo ========================================
	@echo AVR Build successful: $@
	@echo ========================================

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
	@echo Cleaned up.

# Flash (Assuming Arduino Uno - Change PORT as needed)
# Usage: make flash PORT=COM5
PORT ?= COM3
flash: $(HEX)
	$(AVRDUDE) -p $(MCU) -c arduino -P $(PORT) -b 115200 -U flash:w:$(HEX):i

cmake_minimum_required(VERSION 3.10)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)

# Application Name
project(gpio_example_avr C)

# AVR Settings (ATmega328P / Arduino Uno)
set(MCU "atmega328p")
set(F_CPU "16000000UL")
set(BAUD "57600" CACHE STRING "Baud rate for flashing")
set(PORT "COM3" CACHE STRING "Serial port for flashing")

# Toolchain
set(CMAKE_C_COMPILER "avr-gcc")
set(CMAKE_OBJCOPY "avr-objcopy")

# Auto-detect AVRDUDE
set(AVRDUDE_PATH "C:/Users/kuumc/AppData/Local/Arduino15/packages/arduino/tools/avrdude/6.3.0-arduino17/bin/avrdude.exe")
set(AVRDUDE_CONF "C:/Users/kuumc/AppData/Local/Arduino15/packages/arduino/tools/avrdude/6.3.0-arduino17/etc/avrdude.conf")

if(EXISTS ${AVRDUDE_PATH})
    set(AVRDUDE ${AVRDUDE_PATH})
    set(AVRDUDE_FLAGS "-C${AVRDUDE_CONF}")
else()
    set(AVRDUDE "avrdude")
    set(AVRDUDE_FLAGS "")
endif()

# Flags
set(CMAKE_C_FLAGS "-mmcu=${MCU} -DF_CPU=${F_CPU} -DPLATFORM_AVR -Os -Wall -Wextra -std=c99 -ffunction-sections -fdata-sections")
set(CMAKE_EXE_LINKER_FLAGS "-mmcu=${MCU} -Wl,--gc-sections")

# Directories
set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(LOGGING_DIR "${ROOT_DIR}/../logging_driver")
set(HELPER_DIR "${ROOT_DIR}/helpers")
set(IMPL_DIR "${ROOT_DIR}/implementations")
set(COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../common")

# Include Paths
include_directories(
    ${ROOT_DIR}
    ${ROOT_DIR}/config
    ${LOGGING_DIR}
    ${HELPER_DIR}
    ${COMMON_DIR}
    ${ROOT_DIR}/../helper_utils
)

# Config Generation
set(GEN_CONFIG_SRC "${CMAKE_BINARY_DIR}/gpio_config_gen.c")
add_custom_command(
    OUTPUT ${GEN_CONFIG_SRC}
    COMMAND python "${ROOT_DIR}/scripts/gen_config.py" "${CMAKE_CURRENT_SOURCE_DIR}/config.json" "${GEN_CONFIG_SRC}"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/config.json" "${ROOT_DIR}/scripts/gen_config.py"
    COMMENT "Generating GPIO config from JSON..."
)

# Source Files
set(SOURCES
    ${COMMON_DIR}/app_main.c
    ${IMPL_DIR}/avr/platform_adapter.c
    ${ROOT_DIR}/gpioLib.c
    ${GEN_CONFIG_SRC}
    ${HELPER_DIR}/gpio_helper.c
    ${IMPL_DIR}/avr/gpioPlatform_avr.c
    ${LOGGING_DIR}/logLib.c
    ${LOGGING_DIR}/implementations/logPlatform_console.c
)

# Executable
add_executable(${PROJECT_NAME}.elf ${SOURCES})

# HEX Generation
add_custom_command(
    TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex -R .eeprom ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
    COMMENT "Generating HEX file: ${PROJECT_NAME}.hex"
)

# Flash Target
add_custom_target(flash
    COMMAND ${AVRDUDE} ${AVRDUDE_FLAGS} -v -p m328p -c arduino -P ${PORT} -b ${BAUD} -D -U flash:w:${PROJECT_NAME}.hex:i
    DEPENDS ${PROJECT_NAME}.elf
    COMMENT "Flashing ${PROJECT_NAME}.hex to ${PORT}..."
)

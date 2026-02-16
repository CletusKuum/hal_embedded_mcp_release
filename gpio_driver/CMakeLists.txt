cmake_minimum_required(VERSION 3.15)
project(gpio_driver LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Includes
include_directories(
    .
    ../timing_driver
    ../logging_driver
)

# Source groups for IDEs
source_group("Core" FILES gpioLib.c gpioLib.h common.h)
source_group("Config" FILES config/gpio_config.c)
source_group("Implementations" FILES implementations/gpioLib_windows.c implementations/gpioLib_http.c)
source_group("Dependencies" FILES ../logging_driver/logLib.c ../logging_driver/implementations/logPlatform_console.c)

# Executable sources
set(GPIO_SRCS
    gpioLib.c
    config/gpio_config.c
    implementations/gpioLib_windows.c
    implementations/gpioLib_http.c
    ../logging_driver/logLib.c
    ../logging_driver/implementations/logPlatform_console.c
    examples/example_main.c
)

add_executable(gpio_example ${GPIO_SRCS})

# Linking
if(WIN32)
    target_link_libraries(gpio_example ws2_32 winhttp)
endif()

# Installation rules (optional)
# install(TARGETS gpio_example DESTINATION bin)

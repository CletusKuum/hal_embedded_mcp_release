cmake_minimum_required(VERSION 3.15)
project(gpio_example_pc LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Definitions (Default to HTTP)
if(NOT DEFINED PLATFORM)
    set(PLATFORM "HTTP")
endif()

if(PLATFORM STREQUAL "HTTP")
    add_compile_definitions(PLATFORM_HTTP)
    set(IMPL_SRC ../../implementations/pc/gpioLib_http.c)
elseif(PLATFORM STREQUAL "WINDOWS")
    add_compile_definitions(PLATFORM_WINDOWS)
    set(IMPL_SRC ../../implementations/pc/gpioLib_windows.c)
endif()

# Includes
include_directories(
    ../../
    ../../../logging_driver
    ../common
)

# Sources
set(GPIO_SRCS
    ../../gpioLib.c
    ../../config/gpio_config.c
    ${IMPL_SRC}
    ../../../logging_driver/logLib.c
    ../../../logging_driver/implementations/logPlatform_console.c
    ../common/app_main.c
    ../../implementations/pc/platform_adapter.c
)

add_executable(gpio_example_pc ${GPIO_SRCS})

# Linking
if(WIN32)
    target_link_libraries(gpio_example_pc ws2_32 winhttp)
endif()

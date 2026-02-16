cmake_minimum_required(VERSION 3.10)
project(logging_driver C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Source files
set(LOG_SOURCES
    logLib.c
    implementations/logPlatform_console.c
    implementations/logPlatform_file.c
    implementations/logPlatform_http.c
)

# Example main
set(EXAMPLE_SOURCE examples/example_main.c)

# Include directories
include_directories(.)
include_directories(implementations)

# Create library
add_library(logging_lib STATIC ${LOG_SOURCES})

# Create executable
add_executable(example_logging ${EXAMPLE_SOURCE})

# Link library
target_link_libraries(example_logging logging_lib)

# Set output directory
set_target_properties(example_logging PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

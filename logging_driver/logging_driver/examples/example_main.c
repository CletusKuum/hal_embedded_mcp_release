//==============================================================================
// Logging Library - Optimized Example
//------------------------------------------------------------------------------
//! @file
//! @brief Example demonstrating optimized production-ready logging
//------------------------------------------------------------------------------

#include <stdio.h>
#include "../logLib.h"

// Define layer constants
#define PROTOCOL_LAYER "PROTOCOL_LAYER"
#define GPIO_LAYER "GPIO_LAYER"
#define TIMER_LAYER "TIMER_LAYER"
#define NETWORK_LAYER "NETWORK_LAYER"

int main(void)
{
    // Initialize with configuration
    sLogConfig_t sConfig = {
        .eLevel = LOG_LEVEL_INFO,
        .eMode = LOG_MODE_CONSOLE,
        .pcBuildVersion = "1.0.0",
        .bThreadSafe = false
    };
    
    if (eLogInit(&sConfig) != RET_TYPE_SUCCESS) {
        printf("ERROR: Failed to initialize logging!\n");
        return 1;
    }
    
    printf("=== Optimized Logging Example ===\n\n");
    
    // Structured logging with automatic metadata
    LOG_ERROR(PROTOCOL_LAYER, RET_TYPE_NULL_POINTER, "Null pointer in function %s", "eProcessMessage");
    LOG_WARNING(GPIO_LAYER, RET_TYPE_FAIL, "Pin %d initialization failed", 5);
    LOG_INFO(TIMER_LAYER, RET_TYPE_SUCCESS, "Timer started with period %d ms", 1000);
    LOG_INFO(NETWORK_LAYER, RET_TYPE_SUCCESS, "Connection established to %s", "192.168.1.1");
    
    // Debug logging (simpler format)
    vLogSetLevel(LOG_LEVEL_DEBUG);
    LOG_DEBUG("Processing transaction ID: %d", 12345);
    LOG_DEBUG("State machine: %s -> %s", "IDLE", "RUNNING");
    
    // Change log level at runtime
    printf("\n--- Changing log level to WARNING (INFO/DEBUG filtered) ---\n");
    vLogSetLevel(LOG_LEVEL_WARNING);
    LOG_ERROR(PROTOCOL_LAYER, RET_TYPE_FAIL, "This ERROR will be shown");
    LOG_WARNING(GPIO_LAYER, RET_TYPE_FAIL, "This WARNING will be shown");
    LOG_INFO(TIMER_LAYER, RET_TYPE_SUCCESS, "This INFO will be filtered");
    LOG_DEBUG("This DEBUG will be filtered");
    
    // Cleanup
    vLogCleanup();
    
    printf("\n=== Example completed successfully! ===\n");
    return 0;
}

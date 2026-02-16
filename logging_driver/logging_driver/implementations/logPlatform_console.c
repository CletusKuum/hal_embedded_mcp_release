//==============================================================================
// Logging Library - Console Platform Implementation
//------------------------------------------------------------------------------
//! @file
//! @brief Console logging implementation using printf/fprintf
//! 
//! This implementation uses standard C console output (printf/fprintf).
//! Works on Windows, Linux, and other platforms with console support.
//------------------------------------------------------------------------------

#include "logPlatform_console.h"
#include "../logLib.h"
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// ============================================================================
// Console Logging Implementation
// ============================================================================

/**
 * @brief Initialize console logging
 * 
 * Automatically initializes the logging system with console configuration.
 */
static void vConsoleLogInit(void)
{
    // Initialize logging system with console configuration
    sLogConfig_t sLogConfig = {
        .eLevel = LOG_LEVEL_INFO,
        .eMode = LOG_MODE_CONSOLE,
        .pcBuildVersion = "1.0.0",
        .bThreadSafe = false
    };
    
    eLogInit(&sLogConfig);
}

/**
 * @brief Output log message to console
 * 
 * @param pcMessage Formatted log message (JSON or plain text)
 */
static void vConsoleLogOutput(const char *pcMessage)
{
    if (pcMessage == NULL)
        return;
    
    // Output to stderr (standard for logging)
    fprintf(stderr, "%s\n", pcMessage);
    fflush(stderr);
}

/**
 * @brief Cleanup console logging
 */
static void vConsoleLogCleanup(void)
{
    // Console output doesn't need cleanup
}

// ============================================================================
// Console Logging Interface Registration
// ============================================================================

const sLogInterface_t sLogInterfaceConsole = {
    .vHalLogInitFunc = vConsoleLogInit,
    .vHalLogOutputFunc = vConsoleLogOutput,
    .vHalLogCleanupFunc = vConsoleLogCleanup,
};

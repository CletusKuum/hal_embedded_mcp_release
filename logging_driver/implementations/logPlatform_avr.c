//==============================================================================
// Logging Library - AVR Platform Implementation
//------------------------------------------------------------------------------
//! @file
//! @brief AVR logging implementation using printf (UART redirect)
//! 
//! This implementation uses printf for logging output.
//! Requires printf to be redirected to UART (via stdio.h setup).
//------------------------------------------------------------------------------

#include "logPlatform_avr.h"

#ifdef PLATFORM_AVR

#include <stdio.h>
#include <string.h>

// ============================================================================
// AVR Logging Implementation
// ============================================================================

/**
 * @brief Initialize AVR logging
 * 
 * Initializes the logging system with AVR configuration.
 * Note: UART must be initialized separately for printf to work.
 */
static void vAvrLogInit(void)
{
    // Initialize logging system with AVR configuration
    sLogConfig_t sLogConfig = {
        .eLevel = LOG_LEVEL_INFO,
        .eMode = LOG_MODE_CONSOLE,  // UART is treated as console
        .pcBuildVersion = "1.0.0-avr",
        .bThreadSafe = false
    };
    
    eLogInit(&sLogConfig);
}

/**
 * @brief Output log message to AVR UART (via printf)
 * 
 * @param pcMessage Formatted log message (JSON or plain text)
 */
static void vAvrLogOutput(const char *pcMessage)
{
    if (pcMessage == NULL)
        return;
    
    // Output via printf (redirected to UART)
    printf("%s\r\n", pcMessage);
    fflush(stdout); // Ensure data is sent
}

/**
 * @brief Cleanup AVR logging
 */
static void vAvrLogCleanup(void)
{
    // UART doesn't need cleanup on AVR
}

// ============================================================================
// AVR Logging Interface Registration
// ============================================================================

const sLogInterface_t sLogInterfaceAVR = {
    .vHalLogInitFunc = vAvrLogInit,
    .vHalLogOutputFunc = vAvrLogOutput,
    .vHalLogCleanupFunc = vAvrLogCleanup,
};

#endif // PLATFORM_AVR

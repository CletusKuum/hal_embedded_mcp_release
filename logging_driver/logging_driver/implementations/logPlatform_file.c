//==============================================================================
// Logging Library - File Platform Implementation
//------------------------------------------------------------------------------
//! @file
//! @brief File logging implementation using fprintf
//! 
//! This implementation writes logs to a file using standard C file I/O.
//! Works on all platforms with file system support.
//------------------------------------------------------------------------------

#include "logPlatform_file.h"
#include "../logLib.h"
#include <stdio.h>
#include <string.h>

// Static Variables ============================================================

static FILE *g_psLogFile = NULL;
static char g_acLogFilePath[256] = "app.log";

// ============================================================================
// File Logging Implementation
// ============================================================================

/**
 * @brief Initialize file logging
 * 
 * Automatically initializes the logging system with file configuration.
 */
static void vFileLogInit(void)
{
    // Initialize logging system with file configuration
    sLogConfig_t sLogConfig = {
        .eLevel = LOG_LEVEL_INFO,
        .eMode = LOG_MODE_FILE,
        .pcFilePath = "app.log",
        .pcBuildVersion = "1.0.0",
        .bThreadSafe = false
    };
    
    eLogInit(&sLogConfig);
}

/**
 * @brief Output log message to file
 * 
 * @param pcMessage Formatted log message (JSON or plain text)
 */
static void vFileLogOutput(const char *pcMessage)
{
    if (pcMessage == NULL)
        return;
    
    // Open file if not already open
    if (g_psLogFile == NULL) {
        g_psLogFile = fopen(g_acLogFilePath, "a");
        if (g_psLogFile == NULL) {
            return; // Failed to open file
        }
    }
    
    // Write message to file
    fprintf(g_psLogFile, "%s\n", pcMessage);
    fflush(g_psLogFile); // Ensure data is written
}

/**
 * @brief Cleanup file logging
 */
static void vFileLogCleanup(void)
{
    if (g_psLogFile != NULL) {
        fclose(g_psLogFile);
        g_psLogFile = NULL;
    }
}

// ============================================================================
// File Logging Interface Registration
// ============================================================================

const sLogInterface_t sLogInterfaceFile = {
    .vHalLogInitFunc = vFileLogInit,
    .vHalLogOutputFunc = vFileLogOutput,
    .vHalLogCleanupFunc = vFileLogCleanup,
};

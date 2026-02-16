//==============================================================================
// Logging Library - HTTP Platform Implementation
//------------------------------------------------------------------------------
//! @file
//! @brief HTTP logging implementation using curl/system commands
//! 
//! This implementation sends logs via HTTP POST requests using curl.
//! Works on platforms with curl/system command support.
//------------------------------------------------------------------------------

#include "logPlatform_http.h"
#include "../logLib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// Static Variables ============================================================

static char g_acHttpEndpoint[512] = "http://localhost:8080/log";

// ============================================================================
// HTTP Logging Implementation
// ============================================================================

/**
 * @brief Initialize HTTP logging
 * 
 * Automatically initializes the logging system with HTTP configuration.
 */
static void vHttpLogInit(void)
{
    // Initialize logging system with HTTP configuration
    sLogConfig_t sLogConfig = {
        .eLevel = LOG_LEVEL_INFO,
        .eMode = LOG_MODE_HTTP,
        .pcHttpEndpoint = "http://localhost:8080/log",
        .pcBuildVersion = "1.0.0",
        .bThreadSafe = false
    };
    
    eLogInit(&sLogConfig);
}

/**
 * @brief Output log message via HTTP
 * 
 * @param pcMessage Formatted log message (JSON or plain text)
 */
static void vHttpLogOutput(const char *pcMessage)
{
    if (pcMessage == NULL)
        return;
    
    char acCommand[2048];
    char acEscapedMessage[1536];
    
    // Escape message for shell command
    size_t u32Len = strlen(pcMessage);
    size_t u32EscapedIdx = 0;
    for (size_t i = 0; i < u32Len && u32EscapedIdx < sizeof(acEscapedMessage) - 1; i++) {
        if (pcMessage[i] == '\'') {
            acEscapedMessage[u32EscapedIdx++] = '\'';
            acEscapedMessage[u32EscapedIdx++] = '\\';
            acEscapedMessage[u32EscapedIdx++] = '\'';
            acEscapedMessage[u32EscapedIdx++] = '\'';
        } else {
            acEscapedMessage[u32EscapedIdx++] = pcMessage[i];
        }
    }
    acEscapedMessage[u32EscapedIdx] = '\0';
    
    // Build curl command
#ifdef _WIN32
    snprintf(acCommand, sizeof(acCommand),
             "curl -s -X POST -H \"Content-Type: application/json\" -d \"{\\\"log\\\":\\\"%s\\\"}\" %s >nul 2>&1",
             acEscapedMessage, g_acHttpEndpoint);
#else
    snprintf(acCommand, sizeof(acCommand),
             "curl -s -X POST -H \"Content-Type: application/json\" -d '{\"log\":\"%s\"}' %s > /dev/null 2>&1",
             acEscapedMessage, g_acHttpEndpoint);
#endif
    
    system(acCommand);
}

/**
 * @brief Cleanup HTTP logging
 */
static void vHttpLogCleanup(void)
{
    // HTTP logging doesn't need cleanup
}

// ============================================================================
// HTTP Logging Interface Registration
// ============================================================================

const sLogInterface_t sLogInterfaceHTTP = {
    .vHalLogInitFunc = vHttpLogInit,
    .vHalLogOutputFunc = vHttpLogOutput,
    .vHalLogCleanupFunc = vHttpLogCleanup,
};

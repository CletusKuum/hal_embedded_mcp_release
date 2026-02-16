//==============================================================================
// Logging Library - Optimized Production-Ready Implementation
//------------------------------------------------------------------------------
//! @file
//! @brief Expert-level logging library with structured JSON output
//! 
//! Features:
//! - Structured JSON logs with automatic metadata (RFC 3339 timestamps, source location)
//! - Zero-cost when disabled (compile-time filtering)
//! - Efficient buffer management (stack-based, no allocations)
//! - Thread-safe option
//! - Production-ready with error code integration
//------------------------------------------------------------------------------

#ifndef LOG_LIB_H
#define LOG_LIB_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

// Configuration ================================================================

/**
 * @brief Enable structured JSON logging (default: enabled)
 * Set to 0 to disable JSON formatting (plain text output)
 */
#ifndef LOG_LIB_ENABLE_JSON
#define LOG_LIB_ENABLE_JSON 1
#endif

/**
 * @brief Output format style
 * 0 = Compact JSON (single line)
 * 1 = Pretty-printed formatted (multi-line, human-readable)
 */
#ifndef LOG_LIB_FORMAT_STYLE
#define LOG_LIB_FORMAT_STYLE 1
#endif

/**
 * @brief Enable compile-time log level filtering (default: disabled)
 * Set to LOG_LEVEL_* to filter at compile time (zero runtime cost)
 */
#ifndef LOG_LIB_COMPILE_TIME_LEVEL
#define LOG_LIB_COMPILE_TIME_LEVEL LOG_LEVEL_NONE
#endif

/**
 * @brief Maximum log message size (bytes)
 */
#ifndef LOG_LIB_MAX_MESSAGE_SIZE
#define LOG_LIB_MAX_MESSAGE_SIZE 512
#endif

/**
 * @brief Maximum JSON output size (bytes)
 */
#ifndef LOG_LIB_MAX_JSON_SIZE
#define LOG_LIB_MAX_JSON_SIZE 1024
#endif

// Type Definitions ============================================================

/**
 * @brief Log level enumeration
 */
typedef enum {
    LOG_LEVEL_NONE = 0,      //!< No logging
    LOG_LEVEL_ERROR,         //!< Error messages only
    LOG_LEVEL_WARNING,       //!< Warnings and errors
    LOG_LEVEL_INFO,          //!< Informational messages
    LOG_LEVEL_DEBUG,         //!< Debug messages (verbose)
    LOG_LEVEL_TRACE          //!< Trace messages (most verbose)
} eLogLevel_t;

/**
 * @brief Log output mode
 */
typedef enum {
    LOG_MODE_CONSOLE = 0,    //!< Output to stdout/stderr
    LOG_MODE_FILE,           //!< Output to file
    LOG_MODE_HTTP            //!< Output via HTTP
} eLogMode_t;

/**
 * @brief Logging configuration structure
 */
typedef struct {
    eLogLevel_t eLevel;          //!< Minimum log level
    eLogMode_t eMode;            //!< Output mode (deprecated - use platform interface)
    const char *pcFilePath;      //!< File path (for LOG_MODE_FILE)
    const char *pcHttpEndpoint;  //!< HTTP endpoint (for LOG_MODE_HTTP)
    const char *pcBuildVersion;  //!< Build version string
    bool bThreadSafe;            //!< Enable thread safety
} sLogConfig_t;

/**
 * @brief Logging platform interface structure
 * 
 * Platform-specific implementations wrap native logging functions
 * (Arduino Serial.print(), Windows printf(), file I/O, HTTP, etc.)
 * and provide a unified interface.
 */
typedef struct {
    /**
     * @brief Initialize the logging platform (if needed)
     */
    void (*vHalLogInitFunc)(void);
    
    /**
     * @brief Output a log message to the platform
     * @param pcMessage Formatted log message (JSON or plain text)
     */
    void (*vHalLogOutputFunc)(const char *pcMessage);
    
    /**
     * @brief Cleanup/shutdown the logging platform (if needed)
     */
    void (*vHalLogCleanupFunc)(void);
} sLogInterface_t;

// Function Prototypes =========================================================

/**
 * @brief Register logging platform interface
 * @param psInterface Logging platform interface structure
 */
void vHalRegisterLogInterface(const sLogInterface_t *psInterface);

/**
 * @brief Get registered logging platform interface
 * @return const sLogInterface_t* Registered interface, or NULL if not registered
 */
const sLogInterface_t *psHalGetLogInterface(void);

// Function Prototypes =========================================================

/**
 * @brief Initialize logging system
 * @param psConfig Configuration (NULL for defaults: console, INFO level)
 * @return eRetType_t RET_TYPE_SUCCESS on success
 */
eRetType_t eLogInit(const sLogConfig_t *psConfig);

/**
 * @brief Shutdown logging system
 */
void vLogCleanup(void);

/**
 * @brief Set log level at runtime
 * @param eLevel Minimum log level to output
 */
void vLogSetLevel(eLogLevel_t eLevel);

/**
 * @brief Get current log level
 * @return eLogLevel_t Current log level
 */
eLogLevel_t eLogGetLevel(void);

/**
 * @brief Set build version
 * @param pcVersion Version string (e.g., "1.0.0")
 */
void vLogSetBuildVersion(const char *pcVersion);

// Core Logging Functions ======================================================

/**
 * @brief Internal logging function (use macros instead)
 */
void vLogOutputInternal(eLogLevel_t eLevel, const char *pcLayer, eRetType_t eErrorCode,
                        const char *pcFile, const char *pcFunction, int iLine,
                        const char *pcFormat, ...);

// Convenience Macros =========================================================

/**
 * @brief Layer identifiers (define in your code)
 * 
 * Example:
 *   #define PROTOCOL_LAYER "PROTOCOL_LAYER"
 *   #define GPIO_LAYER "GPIO_LAYER"
 */
#ifndef PROTOCOL_LAYER
#define PROTOCOL_LAYER "PROTOCOL_LAYER"
#endif

#ifndef GPIO_LAYER
#define GPIO_LAYER "GPIO_LAYER"
#endif

#ifndef TIMER_LAYER
#define TIMER_LAYER "TIMER_LAYER"
#endif

#ifndef NETWORK_LAYER
#define NETWORK_LAYER "NETWORK_LAYER"
#endif

/**
 * @brief Structured logging macros with automatic metadata capture
 * 
 * Usage:
 *   LOG_ERROR(PROTOCOL_LAYER, RET_TYPE_NULL_POINTER, "Error: %s", error_msg);
 *   LOG_WARNING(GPIO_LAYER, RET_TYPE_FAIL, "Pin %d failed", pin);
 *   LOG_INFO(TIMER_LAYER, RET_TYPE_SUCCESS, "Timer started");
 *   LOG_DEBUG("Processing %d items", count);
 */
#if LOG_LIB_COMPILE_TIME_LEVEL > LOG_LEVEL_NONE
    // Compile-time filtering enabled
    #define LOG_ERROR(layer, errorCode, ...) \
        ((LOG_LEVEL_ERROR >= LOG_LIB_COMPILE_TIME_LEVEL) ? \
         (void)vLogOutputInternal(LOG_LEVEL_ERROR, layer, errorCode, __FILE__, __func__, __LINE__, __VA_ARGS__) : \
         (void)0)
    
    #define LOG_WARNING(layer, errorCode, ...) \
        ((LOG_LEVEL_WARNING >= LOG_LIB_COMPILE_TIME_LEVEL) ? \
         (void)vLogOutputInternal(LOG_LEVEL_WARNING, layer, errorCode, __FILE__, __func__, __LINE__, __VA_ARGS__) : \
         (void)0)
    
    #define LOG_INFO(layer, errorCode, ...) \
        ((LOG_LEVEL_INFO >= LOG_LIB_COMPILE_TIME_LEVEL) ? \
         (void)vLogOutputInternal(LOG_LEVEL_INFO, layer, errorCode, __FILE__, __func__, __LINE__, __VA_ARGS__) : \
         (void)0)
    
    #define LOG_DEBUG(...) \
        ((LOG_LEVEL_DEBUG >= LOG_LIB_COMPILE_TIME_LEVEL) ? \
         (void)vLogOutputInternal(LOG_LEVEL_DEBUG, NULL, RET_TYPE_SUCCESS, __FILE__, __func__, __LINE__, __VA_ARGS__) : \
         (void)0)
#else
    // Runtime filtering (default)
    #define LOG_ERROR(layer, errorCode, ...) \
        vLogOutputInternal(LOG_LEVEL_ERROR, layer, errorCode, __FILE__, __func__, __LINE__, __VA_ARGS__)
    
    #define LOG_WARNING(layer, errorCode, ...) \
        vLogOutputInternal(LOG_LEVEL_WARNING, layer, errorCode, __FILE__, __func__, __LINE__, __VA_ARGS__)
    
    #define LOG_INFO(layer, errorCode, ...) \
        vLogOutputInternal(LOG_LEVEL_INFO, layer, errorCode, __FILE__, __func__, __LINE__, __VA_ARGS__)
    
    #define LOG_DEBUG(...) \
        vLogOutputInternal(LOG_LEVEL_DEBUG, NULL, RET_TYPE_SUCCESS, __FILE__, __func__, __LINE__, __VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif // LOG_LIB_H

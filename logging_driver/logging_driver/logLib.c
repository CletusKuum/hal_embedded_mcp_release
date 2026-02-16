//==============================================================================
// Logging Library - Optimized Implementation
//------------------------------------------------------------------------------
//! @file
//! @brief Production-ready logging implementation with structured JSON output
//------------------------------------------------------------------------------

#include "logLib.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#if defined(_WIN32)
#include <windows.h>
#elif defined(__unix__) || defined(__APPLE__)
#include <pthread.h>
#include <unistd.h>
#else
/* No threading support on bare-metal targets (AVR). */
#endif

// Static Variables ============================================================

static eLogLevel_t g_eCurrentLogLevel = LOG_LEVEL_INFO;
static eLogMode_t g_eLogMode = LOG_MODE_CONSOLE;
static FILE *g_psLogFile = NULL;
static char g_acLogFilePath[256] = "app.log";
static char g_acHttpEndpoint[512] = "http://localhost:8080/log";
static char g_acBuildVersion[32] = "unknown";
static bool g_bInitialized = false;
static bool g_bThreadSafe = false;

// Platform interface (registered by user)
static const sLogInterface_t *g_psLogInterface = NULL;

#if defined(_WIN32)
static CRITICAL_SECTION g_csLogMutex;
#elif defined(__unix__) || defined(__APPLE__)
static pthread_mutex_t g_mutexLog = PTHREAD_MUTEX_INITIALIZER;
#else
/* No mutex on bare-metal targets */
#endif

// Error Code Mapping ==========================================================

typedef struct {
  eRetType_t eErrorCode;
  const char *pcDescription;
} sErrorMapping_t;

static const sErrorMapping_t g_asErrorMapping[] = {
    {RET_TYPE_SUCCESS, "Operation successful"},
    {RET_TYPE_FAIL, "Operation failed"},
    {RET_TYPE_NULL_POINTER, "Null pointer error"},
    {RET_TYPE_INVALID_PARAMETER, "Invalid parameter"},
    {RET_TYPE_MEMORY_ERROR, "Memory allocation failed"},
    {RET_TYPE_NOT_FOUND, "Resource not found"},
    {RET_TYPE_ALREADY_EXISTS, "Resource already exists"},
    {RET_TYPE_NOT_INITIALIZED, "Not initialized"},
    {RET_TYPE_INVALID_STATE, "Invalid state"}};

// Helper Functions ============================================================

static const char *pcGetErrorDescription(eRetType_t eErrorCode) {
  for (size_t i = 0; i < sizeof(g_asErrorMapping) / sizeof(g_asErrorMapping[0]);
       i++) {
    if (g_asErrorMapping[i].eErrorCode == eErrorCode) {
      return g_asErrorMapping[i].pcDescription;
    }
  }
  return "Unknown error";
}

static void vGetRFC3339Timestamp(char *pcBuffer, size_t u32Size) {
  time_t tNow = time(NULL);
  struct tm *psTime = gmtime(&tNow);

  if (psTime != NULL && u32Size >= 21) {
    snprintf(pcBuffer, u32Size, "%04d-%02d-%02dT%02d:%02d:%02dZ",
             psTime->tm_year + 1900, psTime->tm_mon + 1, psTime->tm_mday,
             psTime->tm_hour, psTime->tm_min, psTime->tm_sec);
  } else {
    snprintf(pcBuffer, u32Size, "0000-00-00T00:00:00Z");
  }
}

static const char *pcGetLogLevelString(eLogLevel_t eLevel) {
  switch (eLevel) {
  case LOG_LEVEL_ERROR:
    return "ERROR";
  case LOG_LEVEL_WARNING:
    return "WARNING";
  case LOG_LEVEL_INFO:
    return "INFO";
  case LOG_LEVEL_DEBUG:
    return "DEBUG";
  case LOG_LEVEL_TRACE:
    return "TRACE";
  default:
    return "UNKNOWN";
  }
}

static void vEscapeJsonString(const char *pcInput, char *pcOutput,
                              size_t u32OutputSize) {
  size_t u32InputLen = strlen(pcInput);
  size_t u32OutputIdx = 0;

  for (size_t i = 0; i < u32InputLen && u32OutputIdx < u32OutputSize - 1; i++) {
    switch (pcInput[i]) {
    case '"':
      snprintf(pcOutput + u32OutputIdx, u32OutputSize - u32OutputIdx, "\\\"");
      u32OutputIdx += 2;
      break;
    case '\\':
      snprintf(pcOutput + u32OutputIdx, u32OutputSize - u32OutputIdx, "\\\\");
      u32OutputIdx += 2;
      break;
    case '\n':
      snprintf(pcOutput + u32OutputIdx, u32OutputSize - u32OutputIdx, "\\n");
      u32OutputIdx += 2;
      break;
    case '\r':
      snprintf(pcOutput + u32OutputIdx, u32OutputSize - u32OutputIdx, "\\r");
      u32OutputIdx += 2;
      break;
    case '\t':
      snprintf(pcOutput + u32OutputIdx, u32OutputSize - u32OutputIdx, "\\t");
      u32OutputIdx += 2;
      break;
    default:
      if (u32OutputIdx < u32OutputSize - 1) {
        pcOutput[u32OutputIdx++] = pcInput[i];
      }
      break;
    }
  }
  pcOutput[u32OutputIdx] = '\0';
}

static void vLockMutex(void) {
  if (g_bThreadSafe) {
#if defined(_WIN32)
    EnterCriticalSection(&g_csLogMutex);
#elif defined(__unix__) || defined(__APPLE__)
    pthread_mutex_lock(&g_mutexLog);
#else
    /* No-op on platforms without threading support */
#endif
  }
}

static void vUnlockMutex(void) {
  if (g_bThreadSafe) {
#if defined(_WIN32)
    LeaveCriticalSection(&g_csLogMutex);
#elif defined(__unix__) || defined(__APPLE__)
    pthread_mutex_unlock(&g_mutexLog);
#else
    /* No-op on platforms without threading support */
#endif
  }
}

// Legacy output functions (kept for backward compatibility)
// These are now replaced by platform interface implementations

static void vOutputToConsole(const char *pcMessage) {
  // Use registered platform interface if available
  if (g_psLogInterface != NULL && g_psLogInterface->vHalLogOutputFunc != NULL) {
    g_psLogInterface->vHalLogOutputFunc(pcMessage);
    return;
  }

  // Fallback to default console output
  fprintf(stderr, "%s\n", pcMessage);
  fflush(stderr);
}

static void vOutputToFile(const char *pcMessage) {
#if !defined(PLATFORM_AVR)
  // Use registered platform interface if available
  if (g_psLogInterface != NULL && g_psLogInterface->vHalLogOutputFunc != NULL) {
    g_psLogInterface->vHalLogOutputFunc(pcMessage);
    return;
  }

  // Fallback to default file output
  if (g_psLogFile == NULL) {
    g_psLogFile = fopen(g_acLogFilePath, "a");
    if (g_psLogFile == NULL) {
      // Failed to open file - fallback to console
      vOutputToConsole(pcMessage);
      return;
    }
  }
  fprintf(g_psLogFile, "%s\n", pcMessage);
  fflush(g_psLogFile);
#else
  /* On AVR/bare-metal builds, file I/O is not available; provide a no-op */
  (void)pcMessage;
  /* Optionally route to platform interface if provided */
  if (g_psLogInterface != NULL && g_psLogInterface->vHalLogOutputFunc != NULL) {
    g_psLogInterface->vHalLogOutputFunc(pcMessage);
  }
#endif
}

static void vOutputToHttp(const char *pcMessage) {
  // Use registered platform interface if available
  if (g_psLogInterface != NULL && g_psLogInterface->vHalLogOutputFunc != NULL) {
    g_psLogInterface->vHalLogOutputFunc(pcMessage);
    return;
  }

  // Fallback to default HTTP output
  char acCommand[3072];
  char acEscapedMessage[2048];

  // Escape message for shell command
  size_t u32Len = strlen(pcMessage);
  size_t u32EscapedIdx = 0;
  for (size_t i = 0; i < u32Len && u32EscapedIdx < sizeof(acEscapedMessage) - 1;
       i++) {
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

  snprintf(acCommand, sizeof(acCommand),
           "curl -s -X POST -H \"Content-Type: application/json\" -d "
           "'{\"log\":\"%s\"}' %s > /dev/null 2>&1",
           acEscapedMessage, g_acHttpEndpoint);

  system(acCommand);
}

// Public Functions ============================================================

eRetType_t eLogInit(const sLogConfig_t *psConfig) {
  if (g_bInitialized) {
    return RET_TYPE_ALREADY_EXISTS;
  }

  if (psConfig != NULL) {
    g_eCurrentLogLevel = psConfig->eLevel;
    g_eLogMode = psConfig->eMode;
    g_bThreadSafe = psConfig->bThreadSafe;

    if (psConfig->pcFilePath != NULL) {
      strncpy(g_acLogFilePath, psConfig->pcFilePath,
              sizeof(g_acLogFilePath) - 1);
      g_acLogFilePath[sizeof(g_acLogFilePath) - 1] = '\0';
    }

    if (psConfig->pcHttpEndpoint != NULL) {
      strncpy(g_acHttpEndpoint, psConfig->pcHttpEndpoint,
              sizeof(g_acHttpEndpoint) - 1);
      g_acHttpEndpoint[sizeof(g_acHttpEndpoint) - 1] = '\0';
    }

    if (psConfig->pcBuildVersion != NULL) {
      strncpy(g_acBuildVersion, psConfig->pcBuildVersion,
              sizeof(g_acBuildVersion) - 1);
      g_acBuildVersion[sizeof(g_acBuildVersion) - 1] = '\0';
    }
  }

  if (g_bThreadSafe) {
#if defined(_WIN32)
    InitializeCriticalSection(&g_csLogMutex);
#elif defined(__unix__) || defined(__APPLE__)
    pthread_mutex_init(&g_mutexLog, NULL);
#else
    /* No threading support - nothing to initialize */
#endif
  }

  // Note: Platform interface init is called separately by user
  // (platform implementations call eLogInit internally)

  g_bInitialized = true;
  return RET_TYPE_SUCCESS;
}

void vLogCleanup(void) {
  vLockMutex();

  // Cleanup registered platform interface if available
  if (g_psLogInterface != NULL &&
      g_psLogInterface->vHalLogCleanupFunc != NULL) {
    g_psLogInterface->vHalLogCleanupFunc();
  }

  if (g_psLogFile != NULL) {
    fclose(g_psLogFile);
    g_psLogFile = NULL;
  }

  if (g_bThreadSafe) {
#if defined(_WIN32)
    DeleteCriticalSection(&g_csLogMutex);
#elif defined(__unix__) || defined(__APPLE__)
    pthread_mutex_destroy(&g_mutexLog);
#else
    /* No threading support - nothing to destroy */
#endif
  }

  g_bInitialized = false;
  vUnlockMutex();
}

// ============================================================================
// Platform Interface Registration
// ============================================================================

void vHalRegisterLogInterface(const sLogInterface_t *psInterface) {
  g_psLogInterface = psInterface;
}

const sLogInterface_t *psHalGetLogInterface(void) { return g_psLogInterface; }

void vLogSetLevel(eLogLevel_t eLevel) {
  vLockMutex();
  g_eCurrentLogLevel = eLevel;
  vUnlockMutex();
}

eLogLevel_t eLogGetLevel(void) { return g_eCurrentLogLevel; }

void vLogSetBuildVersion(const char *pcVersion) {
  if (pcVersion != NULL) {
    vLockMutex();
    strncpy(g_acBuildVersion, pcVersion, sizeof(g_acBuildVersion) - 1);
    g_acBuildVersion[sizeof(g_acBuildVersion) - 1] = '\0';
    vUnlockMutex();
  }
}

void vLogOutputInternal(eLogLevel_t eLevel, const char *pcLayer,
                        eRetType_t eErrorCode, const char *pcFile,
                        const char *pcFunction, int iLine, const char *pcFormat,
                        ...) {
  // Fast path: check level before any work
  if (eLevel > g_eCurrentLogLevel) {
    return;
  }

  if (!g_bInitialized || pcFormat == NULL) {
    return;
  }

  vLockMutex();

  // Format message
  char acMessage[LOG_LIB_MAX_MESSAGE_SIZE];
  va_list args;
  va_start(args, pcFormat);
  vsnprintf(acMessage, sizeof(acMessage), pcFormat, args);
  va_end(args);

#if LOG_LIB_ENABLE_JSON
  // Generate structured JSON
  char acTimestamp[32];
  char acEscapedMessage[LOG_LIB_MAX_MESSAGE_SIZE * 2];
  char acEscapedFile[512];
  char acEscapedFunction[256];
  char acJsonOutput[LOG_LIB_MAX_JSON_SIZE + 1024];

  vGetRFC3339Timestamp(acTimestamp, sizeof(acTimestamp));
  vEscapeJsonString(acMessage, acEscapedMessage, sizeof(acEscapedMessage));
  vEscapeJsonString(pcFile ? pcFile : "unknown", acEscapedFile,
                    sizeof(acEscapedFile));
  vEscapeJsonString(pcFunction ? pcFunction : "unknown", acEscapedFunction,
                    sizeof(acEscapedFunction));

  const char *pcErrorDesc = pcGetErrorDescription(eErrorCode);
  const char *pcLayerStr = pcLayer ? pcLayer : "UNKNOWN";

#if LOG_LIB_FORMAT_STYLE == 1
  // Pretty-printed JSON (multi-line, human-readable, valid JSON)
  snprintf(acJsonOutput, sizeof(acJsonOutput),
           "{\n"
           "  \"timestamp\": \"%s\",\n"
           "  \"log_level\": \"%s\",\n"
           "  \"message\": \"%s\",\n"
           "  \"layer\": \"%s\",\n"
           "  \"source\": {\n"
           "    \"file\": \"%s\",\n"
           "    \"function\": \"%s\",\n"
           "    \"line\": %d\n"
           "  },\n"
           "  \"build_version\": \"%s\",\n"
           "  \"error_code\": %d,\n"
           "  \"error_description\": \"%s\"\n"
           "}",
           acTimestamp, pcGetLogLevelString(eLevel), acEscapedMessage,
           pcLayerStr, acEscapedFile, acEscapedFunction, iLine,
           g_acBuildVersion, (int)eErrorCode, pcErrorDesc);
#else
  // Compact JSON (single line)
  snprintf(acJsonOutput, sizeof(acJsonOutput),
           "{\"timestamp\":\"%s\",\"log_level\":\"%s\",\"message\":\"%s\","
           "\"layer\":\"%s\","
           "\"source\":{\"file\":\"%s\",\"function\":\"%s\",\"line\":%d},"
           "\"build_version\":\"%s\",\"error_code\":%d,\"error_description\":"
           "\"%s\"}",
           acTimestamp, pcGetLogLevelString(eLevel), acEscapedMessage,
           pcLayerStr, acEscapedFile, acEscapedFunction, iLine,
           g_acBuildVersion, (int)eErrorCode, pcErrorDesc);
#endif

  // Output based on mode
  switch (g_eLogMode) {
  case LOG_MODE_FILE:
    vOutputToFile(acJsonOutput);
    break;
  case LOG_MODE_HTTP:
    vOutputToHttp(acJsonOutput);
    break;
  case LOG_MODE_CONSOLE:
  default:
    vOutputToConsole(acJsonOutput);
    break;
  }
#else
  // Plain text output
  char acOutput[LOG_LIB_MAX_MESSAGE_SIZE + 128];
  snprintf(acOutput, sizeof(acOutput), "[%s] [%s] %s",
           pcGetLogLevelString(eLevel), pcLayer ? pcLayer : "UNKNOWN",
           acMessage);

  switch (g_eLogMode) {
  case LOG_MODE_FILE:
    vOutputToFile(acOutput);
    break;
  case LOG_MODE_HTTP:
    vOutputToHttp(acOutput);
    break;
  case LOG_MODE_CONSOLE:
  default:
    vOutputToConsole(acOutput);
    break;
  }
#endif

  vUnlockMutex();
}

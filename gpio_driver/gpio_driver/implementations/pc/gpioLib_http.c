//==============================================================================
// GPIO Library - HTTP GPIO Implementation (Simulator)
//------------------------------------------------------------------------------
//! @file
//! @brief HTTP GPIO implementation for Python simulator
//------------------------------------------------------------------------------

// Includes ====================================================================
#include "../../common.h"
#include "../../config/gpio_config.h" // Configuration for all pins
#include "../../gpioLib.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#include <winhttp.h>
#include <winnls.h>
#else
#include <unistd.h>
#endif

// Constants ===================================================================
#define HTTP_BASE_URL "http://localhost:8080"
#define MAX_URL_LEN 256
#define MAX_RESPONSE_SIZE 1024

// Static Variables ============================================================
static bool g_bHTTPInitialized = false;
static bool g_bHTTPInitMessagePrinted = false;
#ifdef _WIN32
static HINTERNET g_hHTTPSession = NULL; // Reused HTTP session
static HINTERNET g_hHTTPConnect = NULL; // Reused connection to localhost:8080
#endif

// Private Function Prototypes ================================================
static eRetType_t eHTTP_MakeRequest(const char *pcURL, const char *pcMethod,
                                    const char *pcBody, char **ppcResponse);
static eRetType_t eHTTP_GetRequest(const char *pcURL, char **ppcResponse);
static eRetType_t eHTTP_PostRequest(const char *pcURL, const char *pcBody,
                                    char **ppcResponse);

// Forward Declarations =======================================================
eRetType_t eGpioHTTPConfigure(const sGpioConfig_t *psConfig);

// Configuration array is defined in ../config/gpio_config.h
// All implementations (HTTP, Windows, STM32) use the SAME config array

// Functions ===================================================================

/**
 * @brief Initialize the HTTP GPIO interface
 * Tests connection to the server and configures default pins for this
 * implementation
 */
void vGpioHTTPInit(void) {
  // Only initialize once
  if (g_bHTTPInitialized) {
    return;
  }

  // Only print initialization messages once
  if (!g_bHTTPInitMessagePrinted) {
    g_bHTTPInitMessagePrinted = true;

    // Test connection to server
    char *pcTestResponse = NULL;
    printf("[GPIO HTTP] Initializing HTTP GPIO implementation...\n");
    printf("[GPIO HTTP] Connecting to: %s/api/gpio/health\n", HTTP_BASE_URL);

    char acHealthURL[MAX_URL_LEN];
    snprintf(acHealthURL, sizeof(acHealthURL), "%s/api/gpio/health",
             HTTP_BASE_URL);

    if (eHTTP_GetRequest(acHealthURL, &pcTestResponse) == RET_TYPE_SUCCESS &&
        pcTestResponse != NULL) {
      printf("[GPIO HTTP] [OK] Server connection successful!\n");
      printf("[GPIO HTTP] Response: %s\n", pcTestResponse);
      free(pcTestResponse);
    } else {
      printf("[GPIO HTTP] [WARNING] Could not connect to server!\n");
      printf("[GPIO HTTP] Make sure Python simulator is running: python "
             "gpio_simulator.py\n");
      printf("[GPIO HTTP] Will retry pin configuration when server becomes "
             "available.\n");
    }
  }

#ifdef _WIN32
  // Create and cache HTTP session and connection for reuse (much faster!)
  if (g_hHTTPSession == NULL) {
    g_hHTTPSession =
        WinHttpOpen(L"GPIO-Library/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                    WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (g_hHTTPSession != NULL) {
      // Set aggressive timeouts for localhost (should respond in <50ms)
      WinHttpSetTimeouts(g_hHTTPSession, 100, 100, 100, 100);

      // Create connection to localhost:8080 and reuse it
      g_hHTTPConnect = WinHttpConnect(g_hHTTPSession, L"localhost", 8080, 0);
      if (g_hHTTPConnect == NULL) {
        WinHttpCloseHandle(g_hHTTPSession);
        g_hHTTPSession = NULL;
      }
    }
  }
#endif

  g_bHTTPInitialized = true;

  // Configure pins from config file - makes implementation self-contained
  // All pin configurations are read from ../config/gpio_config.h
  printf("[GPIO HTTP] Configuring pins from config...\n");

  // Iterate through configured pins and configure them
  const sGpioPinConfig_t *psPinConfig = g_psGpioPinConfigs;
  uint8_t u8ConfiguredCount = 0;

  while (psPinConfig->pcPinName != NULL) {
    sGpioConfig_t sConfig = {.pcPinName = psPinConfig->pcPinName,
                             .eDirection = psPinConfig->eDirection,
                             .ePull = psPinConfig->ePull};

    eRetType_t eRet = eGpioHTTPConfigure(&sConfig);
    if (eRet == RET_TYPE_SUCCESS) {
      printf("[GPIO HTTP] [OK] %s configured (%s, %s)\n",
             psPinConfig->pcPinName,
             (psPinConfig->eDirection == GPIO_DIR_OUTPUT) ? "OUTPUT" : "INPUT",
             (psPinConfig->ePull == GPIO_PULL_UP)     ? "PULL_UP"
             : (psPinConfig->ePull == GPIO_PULL_DOWN) ? "PULL_DOWN"
                                                      : "NONE");
      u8ConfiguredCount++;
    } else {
      printf("[GPIO HTTP] [WARNING] Failed to configure %s (server may not be "
             "ready yet)\n",
             psPinConfig->pcPinName);
    }

    // Small delay between configure operations to avoid overwhelming server
#ifdef _WIN32
    Sleep(10); // 10ms delay
#else
    usleep(10000); // 10ms delay
#endif

    psPinConfig++;
  }

  printf(
      "[GPIO HTTP] Initialization complete. Configured %u pins from config.\n",
      u8ConfiguredCount);
}

/**
 * @brief Configure a GPIO pin via HTTP
 */
eRetType_t eGpioHTTPConfigure(const sGpioConfig_t *psConfig) {
  if (psConfig == NULL || psConfig->pcPinName == NULL) {
    return RET_TYPE_NULL_POINTER;
  }

  if (!g_bHTTPInitialized) {
    return RET_TYPE_NOT_INITIALIZED;
  }

  char acURL[MAX_URL_LEN];
  snprintf(acURL, sizeof(acURL), "%s/api/gpio/%s/configure", HTTP_BASE_URL,
           psConfig->pcPinName);

  char acBody[256];
  snprintf(acBody, sizeof(acBody), "{\"direction\":%d,\"pull\":%d}",
           (int)psConfig->eDirection, (int)psConfig->ePull);

  // Retry logic for configure - sometimes first request after startup times out
  char *pcResponse = NULL;
  eRetType_t eRet = RET_TYPE_FAIL;
  int iRetries = 2; // 2 attempts - localhost should respond fast

  for (int i = 0; i < iRetries; i++) {
    eRet = eHTTP_PostRequest(acURL, acBody, &pcResponse);

    if (eRet == RET_TYPE_SUCCESS) {
      break; // Success - exit retry loop
    }

    // Wait a bit before retry (only if not last attempt)
    if (i < iRetries - 1) {
#ifdef _WIN32
      Sleep(10); // 10ms delay before retry - localhost is fast!
#else
      usleep(10000); // 10ms delay
#endif
      if (pcResponse != NULL) {
        free(pcResponse);
        pcResponse = NULL;
      }
    }
  }

  if (eRet == RET_TYPE_SUCCESS) {
    // Success - empty response is OK for configure operations
    printf("[GPIO HTTP] Pin '%s' configured as %s\n", psConfig->pcPinName,
           psConfig->eDirection == GPIO_DIR_INPUT ? "INPUT" : "OUTPUT");
    if (pcResponse != NULL) {
      free(pcResponse);
    }
  } else {
    printf("[GPIO HTTP] [ERROR] Failed to configure pin '%s' after %d "
           "attempts: %d\n",
           psConfig->pcPinName, iRetries, eRet);
    if (pcResponse != NULL) {
      free(pcResponse);
    }
  }

  return eRet;
}

/**
 * @brief Read a GPIO pin state via HTTP
 */
eRetType_t eGpioHTTPRead(const char *pcPinName, bool *pbValue) {
  if (pcPinName == NULL || pbValue == NULL) {
    return RET_TYPE_NULL_POINTER;
  }

  if (!g_bHTTPInitialized) {
    return RET_TYPE_NOT_INITIALIZED;
  }

  char acURL[MAX_URL_LEN];
  snprintf(acURL, sizeof(acURL), "%s/api/gpio/%s", HTTP_BASE_URL, pcPinName);

  char *pcResponse = NULL;
  eRetType_t eRet = eHTTP_GetRequest(acURL, &pcResponse);

  if (eRet == RET_TYPE_SUCCESS && pcResponse != NULL) {
    // Parse JSON: {"value": 1} or {"value": 0}
    int iValue = 0;
    if (sscanf(pcResponse, "{\"value\":%d}", &iValue) == 1 ||
        sscanf(pcResponse, "{\"value\": %d}", &iValue) == 1) {
      *pbValue = (iValue != 0);
      free(pcResponse);
      return RET_TYPE_SUCCESS;
    }
    free(pcResponse);
  }

  // Fallback: assume LOW if HTTP fails
  *pbValue = false;
  return RET_TYPE_FAIL;
}

/**
 * @brief Write a GPIO pin state via HTTP
 */
eRetType_t eGpioHTTPWrite(const char *pcPinName, bool bValue) {
  if (pcPinName == NULL) {
    return RET_TYPE_NULL_POINTER;
  }

  if (!g_bHTTPInitialized) {
    return RET_TYPE_NOT_INITIALIZED;
  }

  char acURL[MAX_URL_LEN];
  snprintf(acURL, sizeof(acURL), "%s/api/gpio/%s", HTTP_BASE_URL, pcPinName);

  char acBody[128];
  snprintf(acBody, sizeof(acBody), "{\"value\":%d}", bValue ? 1 : 0);

  // Retry logic for write operations (handles transient timeouts)
  char *pcResponse = NULL;
  eRetType_t eRet = RET_TYPE_FAIL;
  int iRetries = 2; // 2 attempts total (1 initial + 1 retry) - localhost should
                    // respond fast

  for (int i = 0; i < iRetries; i++) {
    eRet = eHTTP_PostRequest(acURL, acBody, &pcResponse);

    if (eRet == RET_TYPE_SUCCESS) {
      break; // Success - exit retry loop
    }

    // Wait a bit before retry (only if not last attempt)
    if (i < iRetries - 1) {
#ifdef _WIN32
      Sleep(10); // 10ms delay before retry - localhost is fast!
#else
      usleep(10000); // 10ms delay
#endif
      if (pcResponse != NULL) {
        free(pcResponse);
        pcResponse = NULL;
      }
    }
  }

  if (pcResponse != NULL) {
    free(pcResponse);
  }

  return eRet;
}

// Private Functions ===========================================================

static eRetType_t eHTTP_GetRequest(const char *pcURL, char **ppcResponse) {
  return eHTTP_MakeRequest(pcURL, "GET", NULL, ppcResponse);
}

static eRetType_t eHTTP_PostRequest(const char *pcURL, const char *pcBody,
                                    char **ppcResponse) {
  return eHTTP_MakeRequest(pcURL, "POST", pcBody, ppcResponse);
}

static eRetType_t eHTTP_MakeRequest(const char *pcURL, const char *pcMethod,
                                    const char *pcBody, char **ppcResponse) {
  if (pcURL == NULL || pcMethod == NULL || ppcResponse == NULL) {
    return RET_TYPE_NULL_POINTER;
  }

  *ppcResponse = NULL;

#ifdef _WIN32
  // Parse URL manually (simpler than WinHttpCrackUrl for MinGW compatibility)
  // Expected format: http://localhost:8080/api/gpio/...
  const char *pcPath = strstr(pcURL, "/api/");
  if (pcPath == NULL) {
    return RET_TYPE_FAIL;
  }

  // Use cached session and connection (created during Init) - much faster!
  if (g_hHTTPSession == NULL || g_hHTTPConnect == NULL) {
    return RET_TYPE_NOT_INITIALIZED;
  }

  HINTERNET hConnect = g_hHTTPConnect;

  // Convert path to wide string
  size_t u32PathLen = strlen(pcPath);
  wchar_t *pwPath = (wchar_t *)malloc((u32PathLen + 1) * sizeof(wchar_t));
  if (pwPath == NULL) {
    // Don't close shared handles - they're reused
    return RET_TYPE_MEMORY_ERROR;
  }
  MultiByteToWideChar(CP_UTF8, 0, pcPath, -1, pwPath, (int)(u32PathLen + 1));

  // Convert method to wide string
  size_t u32MethodLen = strlen(pcMethod);
  wchar_t *pwMethod = (wchar_t *)malloc((u32MethodLen + 1) * sizeof(wchar_t));
  if (pwMethod == NULL) {
    free(pwPath);
    // Don't close shared handles - they're reused
    return RET_TYPE_MEMORY_ERROR;
  }
  MultiByteToWideChar(CP_UTF8, 0, pcMethod, -1, pwMethod,
                      (int)(u32MethodLen + 1));

  // Open request
  HINTERNET hRequest =
      WinHttpOpenRequest(hConnect, pwMethod, pwPath, NULL, WINHTTP_NO_REFERER,
                         WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
  free(pwPath);
  free(pwMethod);

  if (hRequest == NULL) {
    // Don't close shared handles - they're reused
    return RET_TYPE_FAIL;
  }

  // Add content type for POST
  if (strcmp(pcMethod, "POST") == 0 && pcBody != NULL) {
    WinHttpAddRequestHeaders(hRequest, L"Content-Type: application/json\r\n",
                             -1, WINHTTP_ADDREQ_FLAG_ADD);
  }

  // Send request
  const void *pvBody =
      (pcBody != NULL) ? (const void *)pcBody : WINHTTP_NO_REQUEST_DATA;
  DWORD dwBodyLength = (pcBody != NULL) ? (DWORD)strlen(pcBody) : 0;

  if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                          (LPVOID)pvBody, dwBodyLength, dwBodyLength, 0)) {
    // Don't log DEBUG messages here - retry logic in calling functions handles
    // errors
    WinHttpCloseHandle(hRequest);
    // Don't close shared handles - they're reused
    return RET_TYPE_FAIL;
  }

  if (!WinHttpReceiveResponse(hRequest, NULL)) {
    // Don't log DEBUG messages here - retry logic in calling functions handles
    // errors
    WinHttpCloseHandle(hRequest);
    // Don't close shared handles - they're reused
    return RET_TYPE_FAIL;
  }

  // Check HTTP status code
  DWORD dwStatusCode = 0;
  DWORD dwStatusCodeSize = sizeof(dwStatusCode);
  if (WinHttpQueryHeaders(hRequest,
                          WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                          NULL, &dwStatusCode, &dwStatusCodeSize, NULL)) {
    // Return failure for non-success status codes (retry logic handles it)
    if (dwStatusCode < 200 || dwStatusCode >= 300) {
      WinHttpCloseHandle(hRequest);
      // Don't close shared handles - they're reused
      return RET_TYPE_FAIL;
    }
  }

  // Read response
  DWORD dwBytesAvailable = 0;
  DWORD dwBytesRead = 0;
  char *pcBuffer = NULL;
  DWORD dwTotalBytesRead = 0;
  DWORD dwBufferSize = MAX_RESPONSE_SIZE;

  pcBuffer = (char *)malloc(dwBufferSize);
  if (pcBuffer == NULL) {
    WinHttpCloseHandle(hRequest);
    // Don't close shared handles - they're reused
    return RET_TYPE_MEMORY_ERROR;
  }

  do {
    if (!WinHttpQueryDataAvailable(hRequest, &dwBytesAvailable))
      break;
    if (dwBytesAvailable == 0)
      break;

    if (dwTotalBytesRead + dwBytesAvailable > dwBufferSize) {
      dwBufferSize = dwTotalBytesRead + dwBytesAvailable + 1;
      char *pcNewBuffer = (char *)realloc(pcBuffer, dwBufferSize);
      if (pcNewBuffer == NULL)
        break;
      pcBuffer = pcNewBuffer;
    }

    if (!WinHttpReadData(hRequest, pcBuffer + dwTotalBytesRead,
                         dwBytesAvailable, &dwBytesRead))
      break;

    dwTotalBytesRead += dwBytesRead;
  } while (dwBytesAvailable > 0);

  // Success - empty response is OK (some endpoints return no body)
  if (dwTotalBytesRead > 0) {
    pcBuffer[dwTotalBytesRead] = '\0';
    *ppcResponse = pcBuffer;
  } else {
    // Empty response - still success (HTTP 200 with no body)
    free(pcBuffer);
    *ppcResponse = NULL;
  }

  WinHttpCloseHandle(hRequest); // Close request handle only
  // Don't close shared session/connection - they're reused for next request
  return RET_TYPE_SUCCESS;

#else
  // Linux/Mac: Use curl via popen
  char acCommand[512];
  if (strcmp(pcMethod, "POST") == 0 && pcBody != NULL) {
    snprintf(acCommand, sizeof(acCommand),
             "curl -s --max-time 0.5 -X POST -H \"Content-Type: "
             "application/json\" -d '%s' \"%s\"",
             pcBody, pcURL);
  } else {
    snprintf(acCommand, sizeof(acCommand), "curl -s --max-time 0.5 \"%s\"",
             pcURL);
  }

  FILE *pFile = popen(acCommand, "r");
  if (pFile != NULL) {
    char acBuffer[MAX_RESPONSE_SIZE] = {0};
    size_t u32TotalRead = 0;
    size_t u32BytesRead;

    while ((u32BytesRead = fread(acBuffer + u32TotalRead, 1,
                                 sizeof(acBuffer) - u32TotalRead - 1, pFile)) >
           0) {
      u32TotalRead += u32BytesRead;
      if (u32TotalRead >= sizeof(acBuffer) - 1)
        break;
    }

    int iStatus = pclose(pFile);

    if (iStatus == 0 && u32TotalRead > 0) {
      *ppcResponse = (char *)malloc(u32TotalRead + 1);
      if (*ppcResponse != NULL) {
        memcpy(*ppcResponse, acBuffer, u32TotalRead);
        (*ppcResponse)[u32TotalRead] = '\0';
        return RET_TYPE_SUCCESS;
      }
    }
  }
#endif

  return RET_TYPE_FAIL;
}

// Export interface structure
const sGpioInterface_t sGpioInterfaceHTTP = {
    .vHalGpioInitFunc = vGpioHTTPInit,
    .eHalGpioConfigureFunc = eGpioHTTPConfigure,
    .eHalGpioReadFunc = eGpioHTTPRead,
    .eHalGpioWriteFunc = eGpioHTTPWrite};

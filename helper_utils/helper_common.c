//==============================================================================
// HAL Helper Utils - Implementation
//------------------------------------------------------------------------------
//! @file
//! @brief Implementation of shared helper functions
//------------------------------------------------------------------------------

#include "helper_common.h"
#include <stdio.h>

/**
 * @brief Send a helper/telemetry message to the bridge
 *
 * Uses printf to send a JSON formatted string.
 * On AVR: This goes to UART (assuming printf redirection is set up).
 * On PC: This goes to stdout.
 */
void vHelperSend(const char *pcCmd, const char *pcPin, int iValue) {
  if (pcCmd != NULL && pcPin != NULL) {
    // JSON Format: {"t":"TYPE","p":"PIN_NAME","v":VALUE}
    printf("{\"t\":\"%s\",\"p\":\"%s\",\"v\":%d}\n", pcCmd, pcPin, iValue);
  }
}

void vHelperSendString(const char *pcCmd, const char *pcPin,
                       const char *pcValue) {
  if (pcCmd != NULL && pcPin != NULL && pcValue != NULL) {
    // JSON Format: {"t":"TYPE","p":"PIN_NAME","v":"VALUE"}
    printf("{\"t\":\"%s\",\"p\":\"%s\",\"v\":\"%s\"}\n", pcCmd, pcPin, pcValue);
  }
}

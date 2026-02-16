/**
 * @file platform_adapter.c
 * @brief AVR Platform Adapter for Universal GPIO Application
 */
#include "../../gpioLib.h" // Explicit relative path
#include "helper_common.h"
#include "implementations/avr/gpioPlatform_avr.h"
#include <avr/interrupt.h> // For ISR
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h> // For uint8_t
#include <stdio.h>
#include <stdlib.h> // For atoi
#include <string.h> // For strstr

#include <util/delay.h>

// ============================================================================
// UART DRIVER (Minimal for Digital Twin Bridge)
// ============================================================================
#define BAUD 57600
#define UBRR_VAL ((F_CPU / 8 / BAUD) - 1) // Using 8 for U2X0 (Double Speed)

static void vUartInit(void) {
  // Set baud rate
  UBRR0H = (unsigned char)(UBRR_VAL >> 8);
  UBRR0L = (unsigned char)UBRR_VAL;
  // Enable Double Speed mode
  UCSR0A |= (1 << U2X0);
  // Enable receiver and transmitter
  // Enable RX Complete Interrupt (RXCIE0) for Digital Twin Input
  UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
  // Set frame format: 8 data bits, 1 stop bit (Standard 8N1)
  UCSR0C = (3 << UCSZ00);
}

// ... (Rest of UART code) ...

// ============================================================================
// DIGITAL TWIN INPUT HANDLING (RX)
// ============================================================================
#define RX_BUFFER_SIZE 128
static char acRxBuffer[RX_BUFFER_SIZE];
static uint8_t u8RxIndex = 0;
/* Pending line: ISR only sets this; main loop dispatches (so printf runs in main). */
static char acPendingLine[RX_BUFFER_SIZE];
static volatile uint8_t u8LineReady = 0;

// Simple Parser: {"t":"GPIO","p":"BUTTON1","v":0}
static void vParseAndApplyInput(char *pcJson) {
  // We expect: "p":"BUTTON1" and "v":0 or 1
  char *pcPinLoc = strstr(pcJson, "\"p\":\"");
  char *pcValLoc = strstr(pcJson, "\"v\":");

  if (pcPinLoc && pcValLoc) {
    // Extract Pin Name
    pcPinLoc += 5; // Skip "p":"
    char acPinName[32];
    uint8_t i = 0;
    while (*pcPinLoc != '"' && i < 31) {
      acPinName[i++] = *pcPinLoc++;
    }
    acPinName[i] = '\0';

    // Extract Value
    pcValLoc += 4; // Skip "v":
    int iValue = atoi(pcValLoc);
    // Note: atoi stops at non-digit (like })

    // Inject into GPIO Driver
    // Use vGpioAVRSetSimulated (from gpioPlatform_avr.h)
    // 1 = Released (High), 0 = Pressed (Low) typically for buttons
    vGpioAVRSetSimulated(acPinName, (iValue != 0));
  }
}

/** Implemented in main (common); dispatches to MCP or DT. */
extern void vOnUartLineReceived(const char *pcLine);

void vApplyReceivedJsonLine(const char *pcLine) {
  if (pcLine != NULL)
    vParseAndApplyInput((char *)pcLine);
}

/* RX ISR: only enqueue line; main loop calls bUartDispatchPendingLine(). */
ISR(USART_RX_vect) {
  char c = UDR0;

  if (c == '\n' || c == '\r') {
    if (u8RxIndex > 0) {
      acRxBuffer[u8RxIndex] = '\0';
      strncpy(acPendingLine, acRxBuffer, RX_BUFFER_SIZE - 1);
      acPendingLine[RX_BUFFER_SIZE - 1] = '\0';
      u8LineReady = 1;
      u8RxIndex = 0;
    }
  } else {
    if (u8RxIndex < RX_BUFFER_SIZE - 1) {
      acRxBuffer[u8RxIndex++] = c;
    } else {
      u8RxIndex = 0;
    }
  }
}

bool bUartDispatchPendingLine(void) {
  if (u8LineReady == 0)
    return false;
  u8LineReady = 0;
  vOnUartLineReceived(acPendingLine);
  return true;
}

void vHelperSend(const char *pcCmd, const char *pcPin, int iValue) {
  // {"t":"<Cmd>","p":"<Pin>","v":<Val>}
  // Manual JSON construction to avoid sprintf bloat (optional, but good for
  // AVR) Using printf since we already redirected it
  printf("{\"t\":\"%s\",\"p\":\"%s\",\"v\":%d}\n", pcCmd, pcPin, iValue);
}

// End of peripheral adapter

// Redefine UART characters
static int uart_putchar(char c, FILE *stream);
static FILE uart_stdout =
    FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

static int uart_putchar(char c, FILE *stream) {
  (void)stream; // Suppress unused parameter warning
  if (c == '\n') {
    uart_putchar('\r', stream);
  }
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = c;
  return 0;
}

// ============================================================================
// PLATFORM CONTRACT IMPLEMENTATION
// ============================================================================

const sGpioInterface_t *psGetPlatformGpioInterface(void) {
  // Ensure UART is ready for logging/helper before GPIO starts
  static bool bInitialized = false;
  if (!bInitialized) {
    vUartInit();
    stdout = &uart_stdout; // Bind printf
    bInitialized = true;
  }
  return &sGpioInterfaceAVR;
}

void vPlatformDelayMs(uint32_t u32Ms) {
  // util/delay.h expects compile-time constant usually, but _delay_ms handles
  // variables (loops) However, for large values it's better to loop
  while (u32Ms--) {
    _delay_ms(1);
  }
}

// ============================================================================

//==============================================================================
// UART Line Received (AVR) - Platform calls main; main dispatches
//------------------------------------------------------------------------------
// ISR calls vOnUartLineReceived(line); main implements it (MCP vs Digital Twin).
// vApplyReceivedJsonLine is implemented here (platform) for DT path.
//------------------------------------------------------------------------------

#ifndef UART_LINE_CALLBACK_H
#define UART_LINE_CALLBACK_H

#ifdef __cplusplus
extern "C" {
#endif

/** Entry point for each complete UART RX line. Implemented in main; dispatches
 *  to MCP (tool_registry) or Digital Twin (vApplyReceivedJsonLine). */
void vOnUartLineReceived(const char *pcLine);

/** Apply a received JSON line (Digital Twin path). Implemented in platform. */
void vApplyReceivedJsonLine(const char *pcLine);

/** Call from main loop to process one pending RX line (run handler in main context). */
bool bUartDispatchPendingLine(void);

#ifdef __cplusplus
}
#endif

#endif /* UART_LINE_CALLBACK_H */

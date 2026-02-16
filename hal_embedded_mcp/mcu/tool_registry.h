//==============================================================================
// HAL Embedded MCP - Tool Registry (MCU)
//------------------------------------------------------------------------------
//! @file
//! @brief Registry of MCP tool names to handlers; parse and dispatch RX lines.
//------------------------------------------------------------------------------

#ifndef TOOL_REGISTRY_H
#define TOOL_REGISTRY_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Handle one line received over UART (non-JSON = MCP command).
 * Parse "tool_name param1 param2 ...", lookup tool, call handler, send response.
 * @param pcLine Null-terminated line (no trailing \\n).
 */
void vMcpHandleLine(const char *pcLine);

#ifdef __cplusplus
}
#endif

#endif /* TOOL_REGISTRY_H */

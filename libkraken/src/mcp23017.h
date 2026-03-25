#ifndef LIBKRAKEN_MCP23017_H
#define LIBKRAKEN_MCP23017_H

#include <stdint.h>

typedef enum mcp23017_iocon_bank : uint8_t {
    MCP23017_IOCON_BANK_0,
    MCP23017_IOCON_BANK_1
} mcp23017_iocon_bank_t;

#endif//LIBKRAKEN_MCP23017_H
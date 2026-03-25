// Copyright 2026 Karma Krafts
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef LIBKRAKEN_MCP23017_H
#define LIBKRAKEN_MCP23017_H

#include <stdint.h>

typedef enum mcp23017_iocon_bank : uint8_t {
    MCP23017_IOCON_BANK_0,
    MCP23017_IOCON_BANK_1
} mcp23017_iocon_bank_t;

#endif//LIBKRAKEN_MCP23017_H
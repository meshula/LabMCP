# cpp20-mcp-template

A minimal template for building MCP (Model Context Protocol) servers in modern C++20.

## Overview

This project provides a working example of an MCP server written in C++20 that implements a dice rolling tool. It's designed to be a starting point for creating your own MCP servers with minimal dependencies and straightforward code.

## Features

- **Roll Dice Tool**: Implements a `roll-dice` tool that accepts notation like `3d8` (roll 3 eight-sided dice)
- **Tool Registry Pattern**: Centralized tool registration with callback-based dispatch
- **Minimal Dependencies**: Only depends on RapidJSON for JSON parsing
- **Modern C++20**: Uses latest C++ features including designated initializers and more
- **Simple Build System**: Uses Make with no external build tools
- **MCP Protocol**: Full implementation of the Model Context Protocol for tool serving
- **Easy to Extend**: Add new tools by creating a class and registering it - no changes to server logic

## Requirements

- C++20 compatible compiler (GCC 10+, Clang 10+, MSVC 2019+)
- Make
- RapidJSON (included in third-party/)

## Building

```bash
git clone https://github.com/yourusername/cpp20-mcp-template.git
cd cpp20-mcp-template
make
```

## Installation

After building, you can install the MCP server for use with Claude Code and/or Claude Desktop.

### Installing for Claude Code

The easiest way to install for Claude Code is using the CLI:

```bash
# From the project directory
claude mcp add cpp-dice -- $(pwd)/build/cpp20-mcp-template
```

This will add the server to your local project configuration. To verify it's working:

```bash
claude mcp list
```

You should see `cpp-dice: ✓ Connected`.

### Installing for Claude Desktop

Add the server to your Claude Desktop configuration file:

**macOS**: `~/Library/Application Support/Claude/claude_desktop_config.json`
**Windows**: `%APPDATA%\Claude\claude_desktop_config.json`
**Linux**: `~/.config/Claude/claude_desktop_config.json`

Add this entry to the `mcpServers` object:

```json
{
  "mcpServers": {
    "cpp-dice": {
      "command": "/absolute/path/to/cpp20-mcp-template/build/cpp20-mcp-template",
      "args": []
    }
  }
}
```

Replace `/absolute/path/to/` with the actual path to your installation. After editing the config, restart Claude Desktop for the changes to take effect.

## Usage

### Running the Server Standalone

You can run the server directly for testing:

```bash
make run
# or
./build/cpp20-mcp-template
```

The server communicates via stdio using the MCP protocol (JSON-RPC 2.0). You can send JSON-RPC requests directly or use the test script:

```bash
make test
```

### Example Tool Usage

The server provides a `roll-dice` tool:

```
roll-dice: Roll dice using standard notation
  - notation (required): Dice notation like "3d8" for 3 eight-sided dice
```

Example:
- `3d8` - Roll 3 eight-sided dice, returns sum between 3-24
- `1d20` - Roll 1 twenty-sided dice, returns 1-20
- `2d6` - Roll 2 six-sided dice, returns 2-12

## Project Structure

```
cpp20-mcp-template/
├── src/
│   ├── main.cpp           # Entry point
│   ├── mcp.hpp            # MCP protocol types
│   ├── mcp.cpp            # MCP protocol implementation
│   ├── server.hpp         # MCP server class
│   ├── server.cpp         # Server implementation
│   └── tools/
│       ├── registry.hpp   # Tool registry infrastructure
│       ├── registry.cpp   # Registry implementation
│       ├── dice.hpp       # Dice rolling tool header
│       └── dice.cpp       # Dice rolling tool implementation
├── third-party/
│   └── rapidjson/         # RapidJSON library
├── Makefile               # Build configuration
├── README.md              # This file
├── PLAN.md                # Technical design
└── STATUS.md              # Current project status
```

## Architecture

The template uses a **tool registry pattern** for maintainable and scalable tool management:

- **`src/tools/registry.hpp/cpp`**: Core infrastructure
  - `ToolDefinition`: Struct containing tool metadata and execution callback
  - `ToolRegistry`: Singleton registry for tool lookup
  - Automatic tool registration via static initializers

- **`src/tools/dice.hpp/cpp`**: Example tool implementation
  - `DICE_NAME`, `DICE_DESCRIPTION`, `DICE_SCHEMA`: Tool metadata constants
  - `executeDiceTool()`: Function matching registry callback signature
  - Core logic: `DiceRoll` and `DiceRollResult` classes
  - Self-registration via static initializer

- **`src/server.hpp/cpp`**: MCP server
  - Generic request routing
  - Uses registry for tool lookup and dispatch
  - No tool-specific code - scales with any number of tools

## Adding New Tools

The registry pattern makes adding tools straightforward:

### Step 1: Create Your Tool Files

Create `src/tools/mytool.hpp`:

```cpp
#pragma once
#include "../mcp.hpp"
#include <rapidjson/document.h>

namespace tools {

// Tool metadata
extern const char* const MYTOOL_NAME;
extern const char* const MYTOOL_DESCRIPTION;
extern const char* const MYTOOL_SCHEMA;

// Tool execution function
mcp::CallToolResult executeMyTool(const rapidjson::Value* arguments);

// Your tool implementation classes/functions...

} // namespace tools
```

Create `src/tools/mytool.cpp`:

```cpp
#include "mytool.hpp"
#include "registry.hpp"

namespace tools {

// Define metadata
const char* const MYTOOL_NAME = "my-tool";
const char* const MYTOOL_DESCRIPTION = "Description of what my tool does";
const char* const MYTOOL_SCHEMA =
    "{\"type\":\"object\",\"properties\":{\"param\":{\"type\":\"string\","
    "\"description\":\"Parameter description\"}},\"required\":[\"param\"]}";

// Automatic registration
namespace {
    struct MyToolRegistrar {
        MyToolRegistrar() {
            ToolRegistry::instance().registerTool(ToolDefinition{
                .name = MYTOOL_NAME,
                .description = MYTOOL_DESCRIPTION,
                .schema_json = MYTOOL_SCHEMA,
                .execute_fn = executeMyTool
            });
        }
    };
    static MyToolRegistrar registrar;
}

// Implement execution
mcp::CallToolResult executeMyTool(const rapidjson::Value* arguments) {
    // Extract arguments
    if (!arguments || !arguments->IsObject()) {
        return mcp::CallToolResult{
            .content = { mcp::Content{ .type = "text", .text = "Missing arguments" } },
            .isError = true
        };
    }

    // Your tool logic here...

    return mcp::CallToolResult{
        .content = { mcp::Content{ .type = "text", .text = "Result here" } },
        .isError = false
    };
}

} // namespace tools
```

### Step 2: Update Makefile

Add your tool source to the `SOURCES` variable:

```makefile
SOURCES = src/main.cpp \
          src/server.cpp \
          src/mcp.cpp \
          src/tools/registry.cpp \
          src/tools/dice.cpp \
          src/tools/mytool.cpp
```

### Step 3: Include in Server

Edit `src/server.cpp` to include your tool (ensures it gets registered):

```cpp
#include "tools/dice.hpp"    // Existing
#include "tools/mytool.hpp"  // Add this
```

### Step 4: Build and Test

```bash
make
make test
```

That's it! The tool will automatically register itself and be available through the MCP server.

## Development

```bash
# Build
make

# Build with debug symbols
make debug

# Run
make run

# Test
make test

# Clean
make clean
```

## Resources

- [MCP Specification](https://modelcontextprotocol.io/)
- [RapidJSON Documentation](https://rapidjson.org/)
- [C++20 Reference](https://en.cppreference.com/)

## License

MIT License - see LICENSE file for details

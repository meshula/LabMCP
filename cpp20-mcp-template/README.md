# cpp20-mcp-template

A minimal template for building MCP (Model Context Protocol) servers in modern C++20.

## Overview

This project provides a working example of an MCP server written in C++20 that implements a dice rolling tool. It's designed to be a starting point for creating your own MCP servers with minimal dependencies and straightforward code.

## Features

- **Roll Dice Tool**: Implements a `roll-dice` tool that accepts notation like `3d8` (roll 3 eight-sided dice)
- **Minimal Dependencies**: Only depends on RapidJSON for JSON parsing
- **Modern C++20**: Uses latest C++ features including concepts, ranges, and more
- **Simple Build System**: Uses Make with no external build tools
- **MCP Protocol**: Full implementation of the Model Context Protocol for tool serving
- **Easy to Extend**: Clean, object-oriented design for adding new tools

## Requirements

- C++20 compatible compiler (GCC 10+, Clang 10+, MSVC 2019+)
- Make
- RapidJSON (included in third-party/)

## Installation

```bash
git clone https://github.com/yourusername/cpp20-mcp-template.git
cd cpp20-mcp-template
make
```

## Usage

### Running the Server

```bash
make run
# or
./build/cpp20-mcp-template
```

The server communicates via stdio using the MCP protocol (JSON-RPC 2.0).

### Using with Claude Desktop

Add to your Claude Desktop configuration:

```json
{
  "mcpServers": {
    "cpp-dice": {
      "command": "/path/to/cpp20-mcp-template/build/cpp20-mcp-template"
    }
  }
}
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
│   ├── main.cpp           # Entry point and stdio handling
│   ├── mcp.hpp            # MCP protocol types
│   ├── mcp.cpp            # MCP protocol implementation
│   ├── server.hpp         # MCP server class
│   ├── server.cpp         # Server implementation
│   └── tools/
│       ├── dice.hpp       # Dice rolling tool header
│       └── dice.cpp       # Dice rolling tool implementation
├── third-party/
│   └── rapidjson/         # RapidJSON library
├── Makefile               # Build configuration
├── README.md              # This file
├── PLAN.md                # Technical design
└── STATUS.md              # Current project status
```

## Creating Your Own MCP Server

1. Clone this repository
2. Modify `src/tools/` to add your own tools
3. Register tools in `src/server.cpp`
4. Update `README.md` with your tool's documentation
5. Build and test with `make && make test`

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

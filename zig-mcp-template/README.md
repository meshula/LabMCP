# zig-mcp-template

A minimal template for building MCP (Model Context Protocol) servers in Zig.

## Overview

This project provides a working example of an MCP server written in Zig that implements a dice rolling tool. It's designed to be a starting point for creating your own MCP servers with minimal dependencies and straightforward code.

## Features

- **Roll Dice Tool**: Implements a `roll-dice` tool that accepts notation like `3d8` (roll 3 eight-sided dice)
- **Minimal Dependencies**: Built with Zig's standard library only
- **Simple Build System**: Uses `zig build` with no external build tools
- **MCP Protocol**: Full implementation of the Model Context Protocol for tool serving
- **Easy to Extend**: Clean, documented code structure for adding new tools

## Requirements

- Zig 0.13.0 or later

## Installation

```bash
git clone https://github.com/yourusername/zig-mcp-template.git
cd zig-mcp-template
zig build
```

## Usage

### Running the Server

```bash
zig build run
```

The server communicates via stdio using the MCP protocol (JSON-RPC 2.0).

### Using with Claude Desktop

Add to your Claude Desktop configuration:

```json
{
  "mcpServers": {
    "zig-dice": {
      "command": "/path/to/zig-mcp-template/zig-out/bin/zig-mcp-template"
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
zig-mcp-template/
├── src/
│   ├── main.zig           # Entry point and stdio handling
│   ├── mcp.zig            # MCP protocol implementation
│   ├── tools/
│   │   └── dice.zig       # Dice rolling tool implementation
│   └── ...
├── build.zig              # Build configuration
├── README.md              # This file
├── PLAN.md                # Technical design and implementation plan
└── STATUS.md              # Current project status
```

## Creating Your Own MCP Server

1. Clone this repository
2. Modify `src/tools/` to add your own tools
3. Update the tool registry in `src/main.zig`
4. Update `README.md` with your tool's documentation
5. Build and test with `zig build run`

## Development

```bash
# Build
zig build

# Run
zig build run

# Test
zig build test

# Clean
rm -rf zig-cache zig-out
```

## Resources

- [MCP Specification](https://modelcontextprotocol.io/)
- [Zig Documentation](https://ziglang.org/documentation/master/)

## License

MIT License - see LICENSE file for details

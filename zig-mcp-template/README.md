# zig-mcp-template

A minimal template for building MCP (Model Context Protocol) servers in Zig.

## Overview

This project provides a working example of an MCP server written in Zig that implements a dice rolling tool. It's designed to be a starting point for creating your own MCP servers with minimal dependencies and straightforward code.

## Features

- **Roll Dice Tool**: Implements a `roll-dice` tool that accepts notation like `3d8` (roll 3 eight-sided dice)
- **Tool Registry Pattern**: Centralized tool registration with function pointer callbacks
- **Minimal Dependencies**: Built with Zig's standard library only
- **Simple Build System**: Uses `zig build` with no external build tools
- **MCP Protocol**: Full implementation of the Model Context Protocol for tool serving
- **Easy to Extend**: Add new tools by creating a module and adding one line to the registry

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
│   ├── mcp.zig            # MCP protocol types and definitions
│   ├── tools/
│   │   ├── registry.zig   # Tool registry infrastructure
│   │   └── dice.zig       # Dice rolling tool implementation
│   └── root.zig
├── build.zig              # Build configuration
├── README.md              # This file
├── PLAN.md                # Technical design and implementation plan
└── STATUS.md              # Current project status
```

## Architecture

The template uses a **tool registry pattern** for maintainable and scalable tool management:

- **`src/tools/registry.zig`**: Core infrastructure
  - `ToolDefinition`: Struct containing tool metadata and execution callback
  - `TOOLS`: Comptime-known array of all registered tools
  - `findToolByName()`: Generic tool lookup
  - `listTools()`: Generate MCP-compatible tool list

- **`src/tools/dice.zig`**: Example tool implementation
  - `NAME`, `DESCRIPTION`, `SCHEMA`: Tool metadata constants
  - `execute()`: Function matching `ToolExecuteFn` signature
  - Core logic: `DiceRoll` and `RollResult` types

- **`src/main.zig`**: MCP server
  - Generic request routing
  - Uses registry for tool lookup and dispatch
  - No tool-specific code - scales with any number of tools

## Adding New Tools

The registry pattern makes adding tools straightforward:

### Step 1: Create Your Tool Module

Create `src/tools/mytool.zig`:

```zig
const std = @import("std");

pub const NAME = "my-tool";
pub const DESCRIPTION = "Description of what my tool does";
pub const SCHEMA =
    \\{
    \\  "type": "object",
    \\  "properties": {
    \\    "param": {
    \\      "type": "string",
    \\      "description": "Parameter description"
    \\    }
    \\  },
    \\  "required": ["param"]
    \\}
;

pub fn execute(
    allocator: std.mem.Allocator,
    arguments: ?std.json.Value,
) ![]const u8 {
    // Extract arguments
    const args_obj = arguments.?.object;
    const param = args_obj.get("param").?.string;

    // Your tool logic here
    return try std.fmt.allocPrint(allocator, "Result: {s}", .{param});
}
```

### Step 2: Register in the Tool Registry

Edit `src/tools/registry.zig`:

```zig
const mytool = @import("mytool.zig");  // Add import

pub const TOOLS = [_]ToolDefinition{
    .{
        .name = dice.NAME,
        .description = dice.DESCRIPTION,
        .schema_json = dice.SCHEMA,
        .execute_fn = &dice.execute,
    },
    // Add your tool here:
    .{
        .name = mytool.NAME,
        .description = mytool.DESCRIPTION,
        .schema_json = mytool.SCHEMA,
        .execute_fn = &mytool.execute,
    },
};
```

### Step 3: Build and Test

```bash
zig build
./test_server.sh
```

That's it! No changes to `main.zig` or dispatch logic needed.

## Development

```bash
# Build
zig build

# Run the server
./zig-out/bin/zig-mcp-template

# Test with integration script
./test_server.sh

# Run unit tests for individual modules
zig test src/tools/dice.zig

# Clean
rm -rf zig-cache zig-out
```

## Resources

- [MCP Specification](https://modelcontextprotocol.io/)
- [Zig Documentation](https://ziglang.org/documentation/master/)

## License

MIT License - see LICENSE file for details

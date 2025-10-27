# LabMCP - MCP Server Templates Collection

A collection of minimal, production-ready templates for building MCP (Model Context Protocol) servers in different languages.

## Overview

This repository contains reference implementations of MCP servers that demonstrate best practices for building tools that integrate with Claude and other MCP-compatible clients. Each template implements a dice rolling tool as a working example.

## Available Templates

### 🦎 zig-mcp-template
**Language:** Zig 0.15.1
**Dependencies:** None (uses only Zig standard library)
**Build System:** zig build

A minimal MCP server implementation in modern Zig, showcasing memory-safe systems programming.

- ✅ Zero external dependencies
- ✅ Fast compilation
- ✅ Type-safe JSON handling
- ✅ Comprehensive test suite

[View Documentation →](./zig-mcp-template/README.md)

### 🔧 cpp20-mcp-template
**Language:** C++20
**Dependencies:** RapidJSON (header-only)
**Build System:** Make

A clean, object-oriented MCP server implementation using modern C++20 features.

- ✅ Modern C++ features (designated initializers, std::optional)
- ✅ RapidJSON for fast JSON parsing
- ✅ RAII and smart pointers
- ✅ Simple Makefile build

[View Documentation →](./cpp20-mcp-template/README.md)

## Quick Start

### Zig Template
```bash
cd zig-mcp-template
zig build
./zig-out/bin/zig-mcp-template
```

### C++ Template
```bash
cd cpp20-mcp-template
make
./build/cpp20-mcp-template
```

## Testing

Both templates include test scripts:

```bash
# Zig
cd zig-mcp-template && ./test_server.sh

# C++
cd cpp20-mcp-template && ./test_server.sh
```

## Using with Claude Desktop

Add to your Claude Desktop configuration (`~/Library/Application Support/Claude/claude_desktop_config.json` on macOS):

```json
{
  "mcpServers": {
    "zig-dice": {
      "command": "/path/to/LabMCP/zig-mcp-template/zig-out/bin/zig-mcp-template"
    },
    "cpp-dice": {
      "command": "/path/to/LabMCP/cpp20-mcp-template/build/cpp20-mcp-template"
    }
  }
}
```

## Features

All templates implement:

- **MCP Protocol**: Full JSON-RPC 2.0 implementation
- **Tool System**: Extensible tool registration and execution
- **Dice Roller**: Example tool accepting notation like "3d8"
- **Error Handling**: Proper error codes and messages
- **stdio Transport**: Standard input/output communication
- **Testing**: Integration test scripts included

## Project Structure

```
LabMCP/
├── README.md                    # This file
├── zig-mcp-template/
│   ├── README.md
│   ├── PLAN.md
│   ├── STATUS.md
│   ├── build.zig
│   ├── src/
│   └── test_server.sh
└── cpp20-mcp-template/
    ├── README.md
    ├── PLAN.md
    ├── STATUS.md
    ├── Makefile
    ├── src/
    ├── third-party/
    └── test_server.sh
```

## Creating Your Own Tool

Both templates are designed to be starting points:

1. Clone or copy a template
2. Modify the tool implementation in `src/tools/`
3. Update the tool registration
4. Build and test
5. Deploy with Claude Desktop

## Resources

- [MCP Specification](https://modelcontextprotocol.io/)
- [Zig Documentation](https://ziglang.org/documentation/master/)
- [C++20 Reference](https://en.cppreference.com/)
- [RapidJSON Documentation](https://rapidjson.org/)

## Language Comparison

| Feature | Zig | C++20 |
|---------|-----|-------|
| Build Speed | ⚡ Very Fast | 🚀 Fast |
| Dependencies | None | RapidJSON only |
| Memory Safety | ✅ Built-in | Manual (RAII) |
| Learning Curve | Moderate | Moderate-High |
| Maturity | Emerging | Mature |
| Performance | Excellent | Excellent |

## Contributing

Each template is self-contained and can be used independently. Feel free to:

- Fork and modify for your needs
- Add new tool implementations
- Submit improvements
- Create templates in other languages

## License

MIT License - see individual template directories for details.

## Support

For questions or issues:
- Check individual template README files
- Review the PLAN.md files for architecture details
- Check STATUS.md files for current state

---

**Built with:** Zig 0.15.1 | C++20 | RapidJSON
**Protocol:** MCP 2024-11-05
**Status:** ✅ Production Ready

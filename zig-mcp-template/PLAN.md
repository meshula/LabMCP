# zig-mcp-template Implementation Plan

## Project Goals

Create a minimal, well-documented template for building MCP servers in Zig that:
1. Demonstrates the MCP protocol implementation
2. Provides a working example tool (dice rolling)
3. Uses only Zig standard library (minimal dependencies)
4. Serves as a starting point for new MCP servers

## Technical Architecture

### Components

#### 1. MCP Protocol Layer (`src/mcp.zig`)
- JSON-RPC 2.0 message handling
- Protocol types and structures:
  - `Request`: JSON-RPC request with id, method, params
  - `Response`: JSON-RPC response with id, result, error
  - `Error`: Standard JSON-RPC error codes
- Message parsing and serialization
- stdio transport implementation

#### 2. Tool Interface (`src/tools.zig`)
- Tool definition structure:
  ```zig
  pub const Tool = struct {
      name: []const u8,
      description: []const u8,
      input_schema: std.json.Value,
      execute: *const fn(params: std.json.Value) anyerror!std.json.Value,
  };
  ```
- Tool registry for managing available tools
- Tool execution dispatcher

#### 3. Dice Rolling Tool (`src/tools/dice.zig`)
- Parse dice notation (e.g., "3d8")
  - Extract number of dice
  - Extract number of sides
  - Validate input
- Random number generation using `std.Random`
- Roll dice and calculate sum
- Return result as JSON

#### 4. Main Server (`src/main.zig`)
- Initialize stdio transport
- Set up tool registry
- Main event loop:
  - Read JSON-RPC messages from stdin
  - Dispatch to appropriate handlers
  - Write responses to stdout
- Handle MCP lifecycle methods:
  - `initialize`: Return server capabilities
  - `tools/list`: Return available tools
  - `tools/call`: Execute tool and return result

### MCP Protocol Methods

#### Required Methods

1. **initialize**
   - Input: `{ protocolVersion: "2024-11-05", capabilities: {} }`
   - Output: `{ protocolVersion, capabilities, serverInfo }`

2. **tools/list**
   - Input: `{}`
   - Output: `{ tools: [{ name, description, inputSchema }] }`

3. **tools/call**
   - Input: `{ name: "roll-dice", arguments: { notation: "3d8" } }`
   - Output: `{ content: [{ type: "text", text: "Result..." }] }`

### Data Flow

```
stdin → JSON Parser → MCP Router → Tool Dispatcher → dice.zig
                                                         ↓
stdout ← JSON Serializer ← Response Builder ← Tool Result
```

## Implementation Phases

### Phase 1: Core Infrastructure
- [ ] Set up `build.zig` with basic configuration
- [ ] Implement JSON-RPC message types
- [ ] Create stdio reader/writer
- [ ] Basic message parsing and routing

### Phase 2: MCP Protocol
- [ ] Implement `initialize` method
- [ ] Implement `tools/list` method
- [ ] Implement `tools/call` method
- [ ] Error handling and validation

### Phase 3: Tool Framework
- [ ] Define `Tool` interface
- [ ] Create tool registry
- [ ] Tool execution dispatcher
- [ ] JSON schema validation

### Phase 4: Dice Tool
- [ ] Parse dice notation (regex or manual parsing)
- [ ] Implement dice rolling logic
- [ ] Random number generation
- [ ] Format results

### Phase 5: Testing & Documentation
- [ ] Unit tests for dice parser
- [ ] Integration tests for MCP protocol
- [ ] Example usage documentation
- [ ] Code comments and API docs

## Technical Decisions

### Why Zig?
- Fast compilation
- No hidden control flow
- Excellent error handling
- Great for systems programming
- Growing ecosystem

### Why Minimal Dependencies?
- Easy to understand and modify
- Faster builds
- Fewer security concerns
- Better template for learning

### Why stdio Transport?
- Simplest MCP transport mechanism
- Works with all MCP clients
- No network complexity
- Perfect for local tools

### JSON Handling
Use `std.json` from Zig standard library:
- `std.json.parseFromSlice` for parsing
- `std.json.stringify` for serialization
- `std.json.Value` for dynamic JSON values

### Random Number Generation
Use `std.Random.DefaultPrng`:
- Seed with timestamp for unpredictability
- Use `random.intRangeAtMost` for dice rolls

## Build Configuration

```zig
// build.zig
const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{
        .name = "zig-mcp-template",
        .root_source_file = b.path("src/main.zig"),
        .target = target,
        .optimize = optimize,
    });

    b.installArtifact(exe);

    // Run command
    const run_cmd = b.addRunArtifact(exe);
    const run_step = b.step("run", "Run the MCP server");
    run_step.dependOn(&run_cmd.step);

    // Test command
    const tests = b.addTest(.{
        .root_source_file = b.path("src/main.zig"),
        .target = target,
        .optimize = optimize,
    });
    const test_step = b.step("test", "Run unit tests");
    test_step.dependOn(&b.addRunArtifact(tests).step);
}
```

## Error Handling Strategy

1. **Parse Errors**: Return JSON-RPC error with code -32700
2. **Invalid Request**: Return error with code -32600
3. **Method Not Found**: Return error with code -32601
4. **Invalid Params**: Return error with code -32602
5. **Internal Error**: Return error with code -32603
6. **Tool Errors**: Return in MCP error format with details

## Testing Strategy

1. **Unit Tests**
   - Dice notation parser
   - JSON message parsing
   - Tool execution

2. **Integration Tests**
   - Full request/response cycle
   - Error handling
   - Tool registry

3. **Manual Testing**
   - Test with MCP client
   - Test with Claude Desktop
   - Verify stdio communication

## Future Enhancements

- [ ] Support for multiple tools in template
- [ ] Resource providers example
- [ ] Prompt templates example
- [ ] WebSocket transport option
- [ ] Configuration file support
- [ ] Logging and debugging tools

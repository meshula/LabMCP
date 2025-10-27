# cpp20-mcp-template Implementation Plan

## Project Goals

Create a minimal, well-documented template for building MCP servers in C++20 that:
1. Demonstrates the MCP protocol implementation
2. Provides a working example tool (dice rolling)
3. Uses only RapidJSON as external dependency
4. Serves as a starting point for new MCP servers

## Technical Architecture

### Components

#### 1. MCP Protocol Layer (`src/mcp.hpp`, `src/mcp.cpp`)
- JSON-RPC 2.0 message handling
- Protocol types and structures:
  ```cpp
  struct Request {
      std::string jsonrpc = "2.0";
      rapidjson::Value id;
      std::string method;
      rapidjson::Value params;
  };

  struct Response {
      std::string jsonrpc = "2.0";
      rapidjson::Value id;
      rapidjson::Value result;
      std::optional<ErrorObject> error;
  };
  ```
- Message parsing and serialization using RapidJSON
- Error handling with standard error codes

#### 2. Server Class (`src/server.hpp`, `src/server.cpp`)
- Main MCP server implementation
- Tool registry using `std::map<std::string, std::unique_ptr<Tool>>`
- Request routing and method dispatch
- stdio transport management
- Lifecycle management (initialize, shutdown)

#### 3. Tool Interface (`src/tools/tool.hpp`)
- Abstract base class for tools:
  ```cpp
  class Tool {
  public:
      virtual ~Tool() = default;
      virtual std::string name() const = 0;
      virtual std::string description() const = 0;
      virtual rapidjson::Value inputSchema() const = 0;
      virtual rapidjson::Value execute(const rapidjson::Value& args) = 0;
  };
  ```

#### 4. Dice Rolling Tool (`src/tools/dice.hpp`, `src/tools/dice.cpp`)
- Parse dice notation using regex or manual parsing
- Random number generation using `<random>`
- Roll dice and calculate sum
- Return detailed results as JSON
- Input validation

#### 5. Main Server (`src/main.cpp`)
- Initialize stdio streams
- Create and configure server instance
- Main event loop reading from stdin
- Error handling and graceful shutdown

### MCP Protocol Methods

#### Required Methods

1. **initialize**
   - Input: `{ protocolVersion, capabilities, clientInfo }`
   - Output: `{ protocolVersion, capabilities, serverInfo }`

2. **tools/list**
   - Input: `{}`
   - Output: `{ tools: [{ name, description, inputSchema }] }`

3. **tools/call**
   - Input: `{ name, arguments }`
   - Output: `{ content: [{ type, text }], isError }`

### Data Flow

```
stdin → Line Reader → JSON Parser → Request Router → Tool Executor
                                                         ↓
stdout ← JSON Writer ← Response Builder ← Tool Result ←─┘
```

## Implementation Phases

### Phase 1: Setup & Dependencies
- [x] Create project directory structure
- [ ] Fetch RapidJSON from GitHub
- [ ] Create Makefile with C++20 support
- [ ] Basic hello world to verify compilation

### Phase 2: Core Infrastructure
- [ ] Implement JSON-RPC types in mcp.hpp
- [ ] Create Server class skeleton
- [ ] Implement stdin/stdout line reader
- [ ] Basic message parsing with RapidJSON

### Phase 3: MCP Protocol
- [ ] Implement `initialize` handler
- [ ] Implement `tools/list` handler
- [ ] Implement `tools/call` dispatcher
- [ ] Error handling and validation

### Phase 4: Tool Framework
- [ ] Define Tool abstract class
- [ ] Implement tool registry
- [ ] Tool execution with error handling
- [ ] JSON schema for tool parameters

### Phase 5: Dice Tool
- [ ] Implement DiceRoll class
- [ ] Parse dice notation (e.g., "3d8")
- [ ] Random number generation
- [ ] Format results as JSON
- [ ] Input validation

### Phase 6: Testing & Polish
- [ ] Create test script
- [ ] Test all MCP methods
- [ ] Memory leak checks (valgrind)
- [ ] Code documentation
- [ ] Example usage

## Technical Decisions

### Why C++20?
- Modern language features (concepts, ranges, etc.)
- Strong type safety
- High performance
- Wide industry adoption
- Great tooling support

### Why RapidJSON?
- Header-only library (easy integration)
- Fast and efficient
- SAX and DOM parsing support
- Well-documented and maintained
- No additional dependencies

### Why Make?
- Simple and ubiquitous
- No additional build tools needed
- Easy to understand and modify
- Works on all Unix-like systems

### JSON Handling
RapidJSON DOM API:
- `rapidjson::Document` for parsing
- `rapidjson::StringBuffer` + `rapidjson::Writer` for serialization
- `rapidjson::Value` for dynamic JSON values

### Random Number Generation
Modern C++ `<random>`:
```cpp
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(1, sides);
```

### Memory Management
- Use RAII principles
- Smart pointers (`std::unique_ptr`, `std::shared_ptr`)
- Avoid manual memory management
- RapidJSON's allocator for JSON values

## Build Configuration

```makefile
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2
INCLUDES = -Ithird-party/rapidjson/include
SOURCES = src/main.cpp src/server.cpp src/mcp.cpp src/tools/dice.cpp
TARGET = build/cpp20-mcp-template

all: $(TARGET)

$(TARGET): $(SOURCES)
    @mkdir -p build
    $(CXX) $(CXXFLAGS) $(INCLUDES) $(SOURCES) -o $(TARGET)
```

## Error Handling Strategy

1. **Parse Errors**: Catch RapidJSON exceptions, return -32700
2. **Invalid Request**: Validate structure, return -32600
3. **Method Not Found**: Check method name, return -32601
4. **Invalid Params**: Validate parameters, return -32602
5. **Internal Error**: Catch tool exceptions, return -32603
6. **Tool Errors**: Return in MCP format with `isError: true`

## Code Style

- Follow C++ Core Guidelines
- Use snake_case for functions and variables
- Use PascalCase for classes
- Const correctness everywhere
- Include guards or `#pragma once`
- Comprehensive comments

## Testing Strategy

1. **Unit Tests**
   - Dice notation parser
   - JSON message parsing
   - Tool execution

2. **Integration Tests**
   - Full request/response cycle
   - All MCP methods
   - Error scenarios

3. **Manual Testing**
   - Test with shell script
   - Memory checks with valgrind
   - Test with Claude Desktop

## Future Enhancements

- [ ] CMake support as alternative to Make
- [ ] Multiple tool examples
- [ ] Resource providers example
- [ ] Prompt templates example
- [ ] WebSocket transport option
- [ ] Configuration file support
- [ ] Logging system
- [ ] Unit test framework (Catch2/GoogleTest)

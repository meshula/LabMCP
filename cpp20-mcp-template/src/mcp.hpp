#pragma once

#include <string>
#include <optional>
#include <vector>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace mcp {

// JSON-RPC 2.0 error codes
enum class ErrorCode : int {
    ParseError = -32700,
    InvalidRequest = -32600,
    MethodNotFound = -32601,
    InvalidParams = -32602,
    InternalError = -32603
};

// JSON-RPC 2.0 Error Object
struct ErrorObject {
    int code;
    std::string message;
    std::optional<rapidjson::Value> data;
};

// MCP Initialize Parameters
struct InitializeParams {
    std::string protocolVersion;
    rapidjson::Value capabilities;
    std::optional<std::string> clientName;
};

// Server Capabilities
struct ServerCapabilities {
    struct ToolsCapability {
        std::optional<bool> listChanged;
    };
    std::optional<ToolsCapability> tools;
};

// Server Info
struct ServerInfo {
    std::string name;
    std::string version;
};

// Initialize Result
struct InitializeResult {
    std::string protocolVersion;
    ServerCapabilities capabilities;
    ServerInfo serverInfo;
};

// Tool Definition
struct Tool {
    std::string name;
    std::string description;
    rapidjson::Document inputSchema;
};

// Tool Call Parameters
struct CallToolParams {
    std::string name;
    std::optional<rapidjson::Value> arguments;
};

// Content item in tool response
struct Content {
    std::string type;
    std::string text;
};

// Tool Call Result
struct CallToolResult {
    std::vector<Content> content;
    bool isError;
};

// Utility functions
std::string serializeToJson(const rapidjson::Value& value);
std::string serializeToJson(const rapidjson::Document& doc);

rapidjson::Document createSuccessResponse(
    rapidjson::Document::AllocatorType& allocator,
    const rapidjson::Value& id,
    const rapidjson::Value& result
);

rapidjson::Document createErrorResponse(
    rapidjson::Document::AllocatorType& allocator,
    const rapidjson::Value& id,
    ErrorCode code,
    const std::string& message
);

rapidjson::Value createInitializeResult(
    rapidjson::Document::AllocatorType& allocator,
    const InitializeResult& result
);

rapidjson::Value createToolsList(
    rapidjson::Document::AllocatorType& allocator,
    const std::vector<Tool>& tools
);

rapidjson::Value createToolCallResult(
    rapidjson::Document::AllocatorType& allocator,
    const CallToolResult& result
);

} // namespace mcp

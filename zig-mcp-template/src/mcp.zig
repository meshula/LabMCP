const std = @import("std");

/// JSON-RPC 2.0 Request
pub const Request = struct {
    jsonrpc: []const u8 = "2.0",
    id: ?std.json.Value = null,
    method: []const u8,
    params: ?std.json.Value = null,
};

/// JSON-RPC 2.0 Response
pub const Response = struct {
    jsonrpc: []const u8 = "2.0",
    id: ?std.json.Value,
    result: ?std.json.Value = null,
    @"error": ?ErrorObject = null,
};

/// JSON-RPC 2.0 Error Object
pub const ErrorObject = struct {
    code: i32,
    message: []const u8,
    data: ?std.json.Value = null,
};

/// Standard JSON-RPC error codes
pub const ErrorCode = enum(i32) {
    ParseError = -32700,
    InvalidRequest = -32600,
    MethodNotFound = -32601,
    InvalidParams = -32602,
    InternalError = -32603,
};

/// MCP Initialize request parameters
pub const InitializeParams = struct {
    protocolVersion: []const u8,
    capabilities: std.json.Value,
    clientInfo: ?ClientInfo = null,
};

/// Client information
pub const ClientInfo = struct {
    name: []const u8,
    version: ?[]const u8 = null,
};

/// MCP Initialize response result
pub const InitializeResult = struct {
    protocolVersion: []const u8,
    capabilities: ServerCapabilities,
    serverInfo: ServerInfo,
};

/// Server capabilities
pub const ServerCapabilities = struct {
    tools: ?ToolsCapability = null,
};

/// Tools capability
pub const ToolsCapability = struct {
    listChanged: ?bool = null,
};

/// Server information
pub const ServerInfo = struct {
    name: []const u8,
    version: []const u8,
};

/// Tool definition
pub const Tool = struct {
    name: []const u8,
    description: []const u8,
    inputSchema: std.json.Value,
};

/// Tools list response
pub const ToolsList = struct {
    tools: []const Tool,
};

/// Tool call request parameters
pub const CallToolParams = struct {
    name: []const u8,
    arguments: ?std.json.Value = null,
};

/// Tool call response result
pub const CallToolResult = struct {
    content: []const Content,
    isError: ?bool = null,
};

/// Content item in tool response
pub const Content = struct {
    type: []const u8,
    text: []const u8,
};

/// Helper to create an error response
pub fn createErrorResponse(allocator: std.mem.Allocator, id: ?std.json.Value, code: ErrorCode, message: []const u8) !Response {
    _ = allocator;
    return Response{
        .id = id,
        .@"error" = ErrorObject{
            .code = @intFromEnum(code),
            .message = message,
        },
    };
}

/// Helper to create a success response
pub fn createSuccessResponse(id: ?std.json.Value, result: std.json.Value) Response {
    return Response{
        .id = id,
        .result = result,
    };
}

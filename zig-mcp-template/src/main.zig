const std = @import("std");
const mcp = @import("mcp.zig");
const registry = @import("tools/registry.zig");

const PROTOCOL_VERSION = "2024-11-05";
const SERVER_NAME = "zig-mcp-template";
const SERVER_VERSION = "0.1.0";

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    const stdin = std.fs.File.stdin();
    const stdout = std.fs.File.stdout();

    var server = Server{
        .allocator = allocator,
        .stdin = stdin,
        .stdout = stdout,
    };

    try server.run();
}

const Server = struct {
    allocator: std.mem.Allocator,
    stdin: std.fs.File,
    stdout: std.fs.File,

    pub fn run(self: *Server) !void {
        var stdout_buffer: [8192]u8 = undefined;
        var stdout_writer = self.stdout.writer(&stdout_buffer);

        // Read line by line from stdin
        var line_buffer: [8192]u8 = undefined;
        var line_len: usize = 0;

        var byte_buffer: [1]u8 = undefined;
        while (true) {
            const bytes_read = self.stdin.read(&byte_buffer) catch |err| {
                if (err == error.EndOfStream) break;
                return err;
            };

            if (bytes_read == 0) break; // EOF

            const byte = byte_buffer[0];
            if (byte == '\n') {
                // Process the complete line
                if (line_len > 0) {
                    const line = line_buffer[0..line_len];
                    const response = self.handleRequest(line) catch |err| {
                        std.debug.print("Error handling request: {}\n", .{err});
                        line_len = 0;
                        continue;
                    };

                    // Write response
                    try stdout_writer.interface.writeAll(response);
                    try stdout_writer.interface.writeAll("\n");
                    try stdout_writer.interface.flush();

                    self.allocator.free(response);
                    line_len = 0;
                }
            } else if (line_len < line_buffer.len) {
                line_buffer[line_len] = byte;
                line_len += 1;
            }
        }
    }

    fn handleRequest(self: *Server, request_json: []const u8) ![]u8 {
        // Parse the JSON-RPC request
        const parsed = try std.json.parseFromSlice(
            std.json.Value,
            self.allocator,
            request_json,
            .{},
        );
        defer parsed.deinit();

        const request_obj = parsed.value.object;
        const method = request_obj.get("method") orelse {
            return self.createErrorResponseJson(null, .InvalidRequest, "Missing method");
        };

        const id = request_obj.get("id");
        const params = request_obj.get("params");

        const method_str = method.string;

        // Route to appropriate handler
        if (std.mem.eql(u8, method_str, "initialize")) {
            return self.handleInitialize(id);
        } else if (std.mem.eql(u8, method_str, "tools/list")) {
            return self.handleToolsList(id);
        } else if (std.mem.eql(u8, method_str, "tools/call")) {
            return self.handleToolsCall(id, params);
        } else {
            return self.createErrorResponseJson(id, .MethodNotFound, "Method not found");
        }
    }

    fn handleInitialize(self: *Server, id: ?std.json.Value) ![]u8 {
        const result = mcp.InitializeResult{
            .protocolVersion = PROTOCOL_VERSION,
            .capabilities = .{
                .tools = .{},
            },
            .serverInfo = .{
                .name = SERVER_NAME,
                .version = SERVER_VERSION,
            },
        };

        return self.createSuccessResponseJson(id, result);
    }

    fn handleToolsList(self: *Server, id: ?std.json.Value) ![]u8 {
        const list = try registry.listTools(self.allocator);
        defer registry.freeToolsList(self.allocator, list);

        const result = mcp.ToolsList{
            .tools = list.tools,
        };

        return self.createSuccessResponseJson(id, result);
    }

    fn handleToolsCall(self: *Server, id: ?std.json.Value, params: ?std.json.Value) ![]u8 {
        const params_obj = if (params) |p| p.object else {
            return self.createErrorResponseJson(id, .InvalidParams, "Missing params");
        };

        const name = params_obj.get("name") orelse {
            return self.createErrorResponseJson(id, .InvalidParams, "Missing tool name");
        };

        const tool_name = name.string;

        // Look up the tool in the registry
        const tool_def = registry.findToolByName(tool_name) orelse {
            return self.createErrorResponseJson(id, .InvalidParams, "Unknown tool");
        };

        // Execute the tool via its callback
        const result_text = tool_def.execute_fn(self.allocator, params_obj.get("arguments")) catch |err| {
            const error_msg = try std.fmt.allocPrint(self.allocator, "Tool execution failed: {}", .{err});
            defer self.allocator.free(error_msg);
            return self.createToolErrorResponse(id, error_msg);
        };
        defer self.allocator.free(result_text);

        // Create the response
        const content = [_]mcp.Content{
            .{
                .type = "text",
                .text = result_text,
            },
        };

        const tool_result = mcp.CallToolResult{
            .content = &content,
            .isError = false,
        };

        return self.createSuccessResponseJson(id, tool_result);
    }

    fn createSuccessResponseJson(self: *Server, id: ?std.json.Value, result: anytype) ![]u8 {
        return try std.json.Stringify.valueAlloc(self.allocator, .{
            .jsonrpc = "2.0",
            .id = id,
            .result = result,
        }, .{});
    }

    fn createErrorResponseJson(self: *Server, id: ?std.json.Value, code: mcp.ErrorCode, message: []const u8) ![]u8 {
        return try std.json.Stringify.valueAlloc(self.allocator, .{
            .jsonrpc = "2.0",
            .id = id,
            .@"error" = .{
                .code = @intFromEnum(code),
                .message = message,
            },
        }, .{});
    }

    fn createToolErrorResponse(self: *Server, id: ?std.json.Value, message: []const u8) ![]u8 {
        const content = [_]mcp.Content{
            .{
                .type = "text",
                .text = message,
            },
        };

        const tool_result = mcp.CallToolResult{
            .content = &content,
            .isError = true,
        };

        return self.createSuccessResponseJson(id, tool_result);
    }
};

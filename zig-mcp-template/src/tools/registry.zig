const std = @import("std");
const mcp = @import("../mcp.zig");
const dice = @import("dice.zig");

/// Function signature for tool execution callbacks
pub const ToolExecuteFn = *const fn (
    allocator: std.mem.Allocator,
    arguments: ?std.json.Value,
) anyerror![]const u8;

/// Definition of a single tool with metadata and execution callback
pub const ToolDefinition = struct {
    name: []const u8,
    description: []const u8,
    schema_json: []const u8,
    execute_fn: ToolExecuteFn,
};

/// Comptime-known array of all registered tools
pub const TOOLS = [_]ToolDefinition{
    .{
        .name = dice.NAME,
        .description = dice.DESCRIPTION,
        .schema_json = dice.SCHEMA,
        .execute_fn = &dice.execute,
    },
};

/// Find a tool by name, returns null if not found
pub fn findToolByName(name: []const u8) ?*const ToolDefinition {
    for (&TOOLS) |*tool| {
        if (std.mem.eql(u8, tool.name, name)) {
            return tool;
        }
    }
    return null;
}

/// Generate an MCP tools list from the registry
pub fn listTools(allocator: std.mem.Allocator) !struct {
    tools: []mcp.Tool,
    schemas: []std.json.Parsed(std.json.Value),
} {
    var tools = try allocator.alloc(mcp.Tool, TOOLS.len);
    errdefer allocator.free(tools);

    var schemas = try allocator.alloc(std.json.Parsed(std.json.Value), TOOLS.len);
    errdefer {
        for (schemas[0..tools.len]) |*schema| {
            schema.deinit();
        }
        allocator.free(schemas);
    }

    for (TOOLS, 0..) |tool_def, i| {
        // Parse the JSON schema for this tool
        const parsed = try std.json.parseFromSlice(
            std.json.Value,
            allocator,
            tool_def.schema_json,
            .{},
        );
        schemas[i] = parsed;

        tools[i] = .{
            .name = tool_def.name,
            .description = tool_def.description,
            .inputSchema = parsed.value,
        };
    }

    return .{
        .tools = tools,
        .schemas = schemas,
    };
}

/// Clean up resources allocated by listTools
pub fn freeToolsList(allocator: std.mem.Allocator, list: anytype) void {
    for (list.schemas) |*schema| {
        schema.deinit();
    }
    allocator.free(list.schemas);
    allocator.free(list.tools);
}

#pragma once

#include "../mcp.hpp"
#include <string>
#include <vector>
#include <functional>
#include <rapidjson/document.h>

namespace tools {

// Function signature for tool execution callbacks
using ToolExecuteFn = std::function<mcp::CallToolResult(const rapidjson::Value*)>;

// Definition of a single tool with metadata and execution callback
struct ToolDefinition {
    std::string name;
    std::string description;
    std::string schema_json;
    ToolExecuteFn execute_fn;
};

// Tool registry class
class ToolRegistry {
public:
    // Get the singleton instance
    static ToolRegistry& instance();

    // Register a tool
    void registerTool(ToolDefinition tool);

    // Find a tool by name
    const ToolDefinition* findToolByName(const std::string& name) const;

    // Get all registered tools
    const std::vector<ToolDefinition>& getTools() const;

    // Generate MCP-compatible tools list
    std::vector<mcp::Tool> listTools() const;

private:
    ToolRegistry() = default;
    std::vector<ToolDefinition> tools_;
};

} // namespace tools

#include "registry.hpp"
#include <algorithm>

namespace tools {

ToolRegistry& ToolRegistry::instance() {
    static ToolRegistry instance;
    return instance;
}

void ToolRegistry::registerTool(ToolDefinition tool) {
    tools_.push_back(std::move(tool));
}

const ToolDefinition* ToolRegistry::findToolByName(const std::string& name) const {
    auto it = std::find_if(tools_.begin(), tools_.end(),
        [&name](const ToolDefinition& tool) {
            return tool.name == name;
        });

    return (it != tools_.end()) ? &(*it) : nullptr;
}

const std::vector<ToolDefinition>& ToolRegistry::getTools() const {
    return tools_;
}

std::vector<mcp::Tool> ToolRegistry::listTools() const {
    std::vector<mcp::Tool> result;
    result.reserve(tools_.size());

    for (const auto& tool_def : tools_) {
        rapidjson::Document schema;
        schema.Parse(tool_def.schema_json.c_str());

        result.push_back(mcp::Tool{
            .name = tool_def.name,
            .description = tool_def.description,
            .inputSchema = std::move(schema)
        });
    }

    return result;
}

} // namespace tools

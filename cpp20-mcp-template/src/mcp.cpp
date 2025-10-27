#include "mcp.hpp"

namespace mcp {

std::string serializeToJson(const rapidjson::Value& value) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    value.Accept(writer);
    return buffer.GetString();
}

std::string serializeToJson(const rapidjson::Document& doc) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    return buffer.GetString();
}

rapidjson::Document createSuccessResponse(
    rapidjson::Document::AllocatorType& allocator,
    const rapidjson::Value& id,
    const rapidjson::Value& result
) {
    rapidjson::Document response;
    response.SetObject();

    response.AddMember("jsonrpc", "2.0", allocator);
    response.AddMember("id", rapidjson::Value(id, allocator), allocator);
    response.AddMember("result", rapidjson::Value(result, allocator), allocator);

    return response;
}

rapidjson::Document createErrorResponse(
    rapidjson::Document::AllocatorType& allocator,
    const rapidjson::Value& id,
    ErrorCode code,
    const std::string& message
) {
    rapidjson::Document response;
    response.SetObject();

    response.AddMember("jsonrpc", "2.0", allocator);
    response.AddMember("id", rapidjson::Value(id, allocator), allocator);

    rapidjson::Value errorObj(rapidjson::kObjectType);
    errorObj.AddMember("code", static_cast<int>(code), allocator);
    errorObj.AddMember("message", rapidjson::Value(message.c_str(), allocator), allocator);

    response.AddMember("error", errorObj, allocator);

    return response;
}

rapidjson::Value createInitializeResult(
    rapidjson::Document::AllocatorType& allocator,
    const InitializeResult& result
) {
    rapidjson::Value resultObj(rapidjson::kObjectType);

    resultObj.AddMember("protocolVersion",
        rapidjson::Value(result.protocolVersion.c_str(), allocator), allocator);

    rapidjson::Value capabilities(rapidjson::kObjectType);
    rapidjson::Value tools(rapidjson::kObjectType);
    capabilities.AddMember("tools", tools, allocator);
    resultObj.AddMember("capabilities", capabilities, allocator);

    rapidjson::Value serverInfo(rapidjson::kObjectType);
    serverInfo.AddMember("name",
        rapidjson::Value(result.serverInfo.name.c_str(), allocator), allocator);
    serverInfo.AddMember("version",
        rapidjson::Value(result.serverInfo.version.c_str(), allocator), allocator);
    resultObj.AddMember("serverInfo", serverInfo, allocator);

    return resultObj;
}

rapidjson::Value createToolsList(
    rapidjson::Document::AllocatorType& allocator,
    const std::vector<Tool>& tools
) {
    rapidjson::Value result(rapidjson::kObjectType);
    rapidjson::Value toolsArray(rapidjson::kArrayType);

    for (const auto& tool : tools) {
        rapidjson::Value toolObj(rapidjson::kObjectType);
        toolObj.AddMember("name",
            rapidjson::Value(tool.name.c_str(), allocator), allocator);
        toolObj.AddMember("description",
            rapidjson::Value(tool.description.c_str(), allocator), allocator);
        toolObj.AddMember("inputSchema",
            rapidjson::Value(tool.inputSchema, allocator), allocator);

        toolsArray.PushBack(toolObj, allocator);
    }

    result.AddMember("tools", toolsArray, allocator);
    return result;
}

rapidjson::Value createToolCallResult(
    rapidjson::Document::AllocatorType& allocator,
    const CallToolResult& result
) {
    rapidjson::Value resultObj(rapidjson::kObjectType);
    rapidjson::Value contentArray(rapidjson::kArrayType);

    for (const auto& item : result.content) {
        rapidjson::Value contentObj(rapidjson::kObjectType);
        contentObj.AddMember("type",
            rapidjson::Value(item.type.c_str(), allocator), allocator);
        contentObj.AddMember("text",
            rapidjson::Value(item.text.c_str(), allocator), allocator);
        contentArray.PushBack(contentObj, allocator);
    }

    resultObj.AddMember("content", contentArray, allocator);
    resultObj.AddMember("isError", result.isError, allocator);

    return resultObj;
}

} // namespace mcp

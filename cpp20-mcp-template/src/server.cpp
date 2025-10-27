#include "server.hpp"
#include "tools/dice.hpp"
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <iostream>
#include <sstream>

namespace mcp {

Server::Server(const std::string& name, const std::string& version)
    : name_(name), version_(version) {}

void Server::run() {
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;

        try {
            std::string response = handleRequest(line);
            std::cout << response << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error handling request: " << e.what() << std::endl;
        }
    }
}

std::string Server::handleRequest(const std::string& request_json) {
    rapidjson::Document doc;
    doc.Parse(request_json.c_str());

    if (doc.HasParseError()) {
        rapidjson::Value null_id;
        null_id.SetNull();
        rapidjson::Document response = createErrorResponse(
            doc.GetAllocator(),
            null_id,
            ErrorCode::ParseError,
            "JSON parse error: " + std::string(rapidjson::GetParseError_En(doc.GetParseError()))
        );
        return serializeToJson(response);
    }

    if (!doc.IsObject() || !doc.HasMember("method")) {
        rapidjson::Value id_val;
        if (doc.HasMember("id")) {
            id_val.CopyFrom(doc["id"], doc.GetAllocator());
        } else {
            id_val.SetNull();
        }
        rapidjson::Document response = createErrorResponse(
            doc.GetAllocator(),
            id_val,
            ErrorCode::InvalidRequest,
            "Missing method field"
        );
        return serializeToJson(response);
    }

    const auto& id = doc["id"];
    const std::string method = doc["method"].GetString();

    if (method == "initialize") {
        return handleInitialize(doc.GetAllocator(), id);
    } else if (method == "tools/list") {
        return handleToolsList(doc.GetAllocator(), id);
    } else if (method == "tools/call") {
        if (!doc.HasMember("params")) {
            rapidjson::Document response = createErrorResponse(
                doc.GetAllocator(), id, ErrorCode::InvalidParams, "Missing params"
            );
            return serializeToJson(response);
        }
        return handleToolsCall(doc.GetAllocator(), id, doc["params"]);
    } else {
        rapidjson::Document response = createErrorResponse(
            doc.GetAllocator(), id, ErrorCode::MethodNotFound, "Method not found"
        );
        return serializeToJson(response);
    }
}

std::string Server::handleInitialize(
    rapidjson::Document::AllocatorType& allocator,
    const rapidjson::Value& id
) {
    InitializeResult init_result{
        .protocolVersion = PROTOCOL_VERSION,
        .capabilities = ServerCapabilities{
            .tools = ServerCapabilities::ToolsCapability{}
        },
        .serverInfo = ServerInfo{
            .name = name_,
            .version = version_
        }
    };

    rapidjson::Value result = createInitializeResult(allocator, init_result);
    rapidjson::Document response = createSuccessResponse(allocator, id, result);
    return serializeToJson(response);
}

std::string Server::handleToolsList(
    rapidjson::Document::AllocatorType& allocator,
    const rapidjson::Value& id
) {
    // Create input schema for roll-dice tool
    const char* schema_json =
        "{\"type\":\"object\",\"properties\":{\"notation\":{\"type\":\"string\","
        "\"description\":\"Dice notation (e.g., '3d8' for 3 eight-sided dice)\"}},"
        "\"required\":[\"notation\"]}";

    rapidjson::Document schema;
    schema.Parse(schema_json);

    std::vector<Tool> tools;
    tools.push_back(Tool{
        .name = "roll-dice",
        .description = "Roll dice using standard notation (e.g., 3d8 for 3 eight-sided dice)",
        .inputSchema = std::move(schema)
    });

    rapidjson::Value result = createToolsList(allocator, tools);
    rapidjson::Document response = createSuccessResponse(allocator, id, result);
    return serializeToJson(response);
}

std::string Server::handleToolsCall(
    rapidjson::Document::AllocatorType& allocator,
    const rapidjson::Value& id,
    const rapidjson::Value& params
) {
    if (!params.IsObject() || !params.HasMember("name")) {
        rapidjson::Document response = createErrorResponse(
            allocator, id, ErrorCode::InvalidParams, "Missing tool name"
        );
        return serializeToJson(response);
    }

    const std::string tool_name = params["name"].GetString();

    if (tool_name == "roll-dice") {
        const rapidjson::Value* arguments = params.HasMember("arguments")
            ? &params["arguments"] : nullptr;
        return executeRollDice(allocator, id, arguments);
    } else {
        rapidjson::Document response = createErrorResponse(
            allocator, id, ErrorCode::InvalidParams, "Unknown tool: " + tool_name
        );
        return serializeToJson(response);
    }
}

std::string Server::executeRollDice(
    rapidjson::Document::AllocatorType& allocator,
    const rapidjson::Value& id,
    const rapidjson::Value* arguments
) {
    if (!arguments || !arguments->IsObject() || !arguments->HasMember("notation")) {
        return createToolErrorResponse(allocator, id, "Missing notation argument");
    }

    const std::string notation = (*arguments)["notation"].GetString();

    try {
        auto result = tools::executeDiceRoll(notation);
        std::string result_text = result.format();

        CallToolResult call_result{
            .content = { Content{ .type = "text", .text = result_text } },
            .isError = false
        };

        rapidjson::Value result_value = createToolCallResult(allocator, call_result);
        rapidjson::Document response = createSuccessResponse(allocator, id, result_value);
        return serializeToJson(response);
    } catch (const tools::InvalidDiceNotation& e) {
        return createToolErrorResponse(allocator, id,
            "Invalid dice notation. Use format like '3d8'. Error: " + std::string(e.what()));
    } catch (const std::exception& e) {
        return createToolErrorResponse(allocator, id,
            "Error rolling dice: " + std::string(e.what()));
    }
}

std::string Server::createToolErrorResponse(
    rapidjson::Document::AllocatorType& allocator,
    const rapidjson::Value& id,
    const std::string& message
) {
    CallToolResult call_result{
        .content = { Content{ .type = "text", .text = message } },
        .isError = true
    };

    rapidjson::Value result_value = createToolCallResult(allocator, call_result);
    rapidjson::Document response = createSuccessResponse(allocator, id, result_value);
    return serializeToJson(response);
}

} // namespace mcp

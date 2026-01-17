#pragma once

#include "mcp.hpp"
#include <string>
#include <iostream>
#include <rapidjson/document.h>

namespace mcp {

class Server {
public:
    Server(const std::string& name, const std::string& version);

    // Main server loop - reads from stdin, writes to stdout
    void run();

private:
    std::string name_;
    std::string version_;
    static constexpr const char* PROTOCOL_VERSION = "2024-11-05";

    // Handle incoming JSON-RPC request
    std::string handleRequest(const std::string& request_json);

    // MCP method handlers
    std::string handleInitialize(
        rapidjson::Document::AllocatorType& allocator,
        const rapidjson::Value& id
    );

    std::string handleToolsList(
        rapidjson::Document::AllocatorType& allocator,
        const rapidjson::Value& id
    );

    std::string handleToolsCall(
        rapidjson::Document::AllocatorType& allocator,
        const rapidjson::Value& id,
        const rapidjson::Value& params
    );
};

} // namespace mcp

#include "server.hpp"
#include <iostream>

int main() {
    try {
        mcp::Server server("cpp20-mcp-template", "0.1.0");
        server.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}

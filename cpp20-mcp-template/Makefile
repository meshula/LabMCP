# Makefile for cpp20-mcp-template

CXX = clang++
CXXFLAGS = -std=c++20 -Wall -Wextra -Wpedantic -O2
CXXFLAGS_DEBUG = -std=c++20 -Wall -Wextra -Wpedantic -g -O0
INCLUDES = -Ithird-party

# Source files
SOURCES = src/main.cpp \
          src/server.cpp \
          src/mcp.cpp \
          src/tools/dice.cpp

# Object files
BUILD_DIR = build
OBJECTS = $(SOURCES:src/%.cpp=$(BUILD_DIR)/%.o)

# Target executable
TARGET = $(BUILD_DIR)/cpp20-mcp-template

# Default target
all: $(TARGET)

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/tools

# Link
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)
	@echo "Build complete: $(TARGET)"

# Compile source files
$(BUILD_DIR)/%.o: src/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Debug build
debug: CXXFLAGS = $(CXXFLAGS_DEBUG)
debug: clean $(TARGET)

# Run the server
run: $(TARGET)
	$(TARGET)

# Test the server
test: $(TARGET)
	@chmod +x test_server.sh
	@./test_server.sh

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)

# Clean everything including dependencies
distclean: clean
	rm -rf third-party/rapidjson

# Help
help:
	@echo "Available targets:"
	@echo "  all (default) - Build the project"
	@echo "  debug         - Build with debug symbols"
	@echo "  run           - Build and run the server"
	@echo "  test          - Run tests"
	@echo "  clean         - Remove build artifacts"
	@echo "  distclean     - Remove build artifacts and dependencies"
	@echo "  help          - Show this help message"

.PHONY: all debug run test clean distclean help

#pragma once

#include "../mcp.hpp"
#include <string>
#include <vector>
#include <random>
#include <stdexcept>
#include <rapidjson/document.h>

namespace tools {

// Tool metadata constants
extern const char* const DICE_NAME;
extern const char* const DICE_DESCRIPTION;
extern const char* const DICE_SCHEMA;

// Tool execution function matching registry signature
mcp::CallToolResult executeDiceTool(const rapidjson::Value* arguments);

// Exception for invalid dice notation
class InvalidDiceNotation : public std::runtime_error {
public:
    explicit InvalidDiceNotation(const std::string& msg)
        : std::runtime_error(msg) {}
};

// Represents a dice roll (e.g., 3d8)
class DiceRoll {
public:
    DiceRoll(unsigned int count, unsigned int sides);

    // Parse dice notation like "3d8"
    static DiceRoll parse(const std::string& notation);

    // Roll the dice and return individual results
    std::vector<unsigned int> roll() const;

    // Roll and return just the sum
    unsigned int rollSum() const;

    unsigned int count() const { return count_; }
    unsigned int sides() const { return sides_; }

private:
    unsigned int count_;
    unsigned int sides_;

    static std::random_device rd_;
    static std::mt19937 gen_;
};

// Result of a dice roll with detailed information
struct DiceRollResult {
    std::string notation;
    std::vector<unsigned int> rolls;
    unsigned int total;

    std::string format() const;
};

// Execute a dice roll from notation string
DiceRollResult executeDiceRoll(const std::string& notation);

} // namespace tools

#include "dice.hpp"
#include <sstream>
#include <algorithm>
#include <numeric>

namespace tools {

// Static random number generator
std::random_device DiceRoll::rd_;
std::mt19937 DiceRoll::gen_(rd_());

DiceRoll::DiceRoll(unsigned int count, unsigned int sides)
    : count_(count), sides_(sides) {
    if (count == 0 || count > 100) {
        throw InvalidDiceNotation("Dice count must be between 1 and 100");
    }
    if (sides == 0 || sides > 1000) {
        throw InvalidDiceNotation("Dice sides must be between 1 and 1000");
    }
}

DiceRoll DiceRoll::parse(const std::string& notation) {
    // Find the 'd' separator
    size_t d_pos = notation.find('d');
    if (d_pos == std::string::npos || d_pos == 0 || d_pos == notation.length() - 1) {
        throw InvalidDiceNotation("Invalid notation format. Use format like '3d8'");
    }

    try {
        std::string count_str = notation.substr(0, d_pos);
        std::string sides_str = notation.substr(d_pos + 1);

        unsigned int count = std::stoul(count_str);
        unsigned int sides = std::stoul(sides_str);

        return DiceRoll(count, sides);
    } catch (const std::invalid_argument&) {
        throw InvalidDiceNotation("Invalid number in dice notation");
    } catch (const std::out_of_range&) {
        throw InvalidDiceNotation("Number out of range in dice notation");
    }
}

std::vector<unsigned int> DiceRoll::roll() const {
    std::vector<unsigned int> results;
    results.reserve(count_);

    std::uniform_int_distribution<unsigned int> dist(1, sides_);

    for (unsigned int i = 0; i < count_; ++i) {
        results.push_back(dist(gen_));
    }

    return results;
}

unsigned int DiceRoll::rollSum() const {
    auto results = roll();
    return std::accumulate(results.begin(), results.end(), 0u);
}

std::string DiceRollResult::format() const {
    std::ostringstream oss;
    oss << "Rolling " << notation << ":\n";
    oss << "Individual rolls: [";

    for (size_t i = 0; i < rolls.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << rolls[i];
    }

    oss << "]\n";
    oss << "Total: " << total;

    return oss.str();
}

DiceRollResult executeDiceRoll(const std::string& notation) {
    DiceRoll dice = DiceRoll::parse(notation);
    auto rolls = dice.roll();
    unsigned int total = std::accumulate(rolls.begin(), rolls.end(), 0u);

    return DiceRollResult{
        .notation = notation,
        .rolls = std::move(rolls),
        .total = total
    };
}

} // namespace tools

const std = @import("std");

/// Tool metadata constants
pub const NAME = "roll-dice";
pub const DESCRIPTION = "Roll dice using standard notation (e.g., 3d8 for 3 eight-sided dice)";
pub const SCHEMA =
    \\{
    \\  "type": "object",
    \\  "properties": {
    \\    "notation": {
    \\      "type": "string",
    \\      "description": "Dice notation (e.g., '3d8' for 3 eight-sided dice)"
    \\    }
    \\  },
    \\  "required": ["notation"]
    \\}
;

/// Execute the dice roll tool
pub fn execute(
    allocator: std.mem.Allocator,
    arguments: ?std.json.Value,
) ![]const u8 {
    // Extract arguments
    const args_obj = if (arguments) |a| a.object else {
        return try allocator.dupe(u8, "Missing arguments");
    };

    const notation_value = args_obj.get("notation") orelse {
        return try allocator.dupe(u8, "Missing notation argument");
    };

    const notation = notation_value.string;

    // Parse and roll the dice
    const roll = DiceRoll.parse(notation) catch {
        return try allocator.dupe(u8, "Invalid dice notation. Use format like '3d8'");
    };

    var prng = std.Random.DefaultPrng.init(@intCast(std.time.timestamp()));
    const random = prng.random();

    var result = try roll.rollDetailed(allocator, random);
    result.notation = try allocator.dupe(u8, notation);
    defer result.deinit(allocator);

    // Format and return the result
    return try result.format(allocator);
}

/// Parse dice notation like "3d8" into number of dice and number of sides
pub const DiceRoll = struct {
    count: u32,
    sides: u32,

    pub fn parse(notation: []const u8) !DiceRoll {
        // Find the 'd' separator
        const d_pos = std.mem.indexOf(u8, notation, "d") orelse {
            return error.InvalidNotation;
        };

        if (d_pos == 0 or d_pos == notation.len - 1) {
            return error.InvalidNotation;
        }

        const count_str = notation[0..d_pos];
        const sides_str = notation[d_pos + 1 ..];

        const count = std.fmt.parseInt(u32, count_str, 10) catch {
            return error.InvalidCount;
        };

        const sides = std.fmt.parseInt(u32, sides_str, 10) catch {
            return error.InvalidSides;
        };

        if (count == 0 or count > 100) {
            return error.InvalidCount;
        }

        if (sides == 0 or sides > 1000) {
            return error.InvalidSides;
        }

        return DiceRoll{
            .count = count,
            .sides = sides,
        };
    }

    /// Roll the dice and return the sum
    pub fn roll(self: DiceRoll, random: std.Random) !u32 {
        var total: u32 = 0;
        var i: u32 = 0;
        while (i < self.count) : (i += 1) {
            const value = random.intRangeAtMost(u32, 1, self.sides);
            total += value;
        }
        return total;
    }

    /// Roll the dice and return detailed results
    pub fn rollDetailed(self: DiceRoll, allocator: std.mem.Allocator, random: std.Random) !RollResult {
        var rolls = try allocator.alloc(u32, self.count);
        var total: u32 = 0;

        var i: u32 = 0;
        while (i < self.count) : (i += 1) {
            const value = random.intRangeAtMost(u32, 1, self.sides);
            rolls[i] = value;
            total += value;
        }

        return RollResult{
            .rolls = rolls,
            .total = total,
            .notation = try allocator.dupe(u8, ""), // Will be set by caller
        };
    }
};

/// Result of a dice roll with detailed information
pub const RollResult = struct {
    rolls: []u32,
    total: u32,
    notation: []const u8,

    pub fn deinit(self: *RollResult, allocator: std.mem.Allocator) void {
        allocator.free(self.rolls);
        allocator.free(self.notation);
    }

    /// Format the result as a string
    pub fn format(self: RollResult, allocator: std.mem.Allocator) ![]u8 {
        // Build the result string manually
        var rolls_str = std.ArrayList(u8){};
        errdefer rolls_str.deinit(allocator);

        for (self.rolls, 0..) |roll_value, i| {
            if (i > 0) {
                try rolls_str.appendSlice(allocator, ", ");
            }
            const num_str = try std.fmt.allocPrint(allocator, "{d}", .{roll_value});
            defer allocator.free(num_str);
            try rolls_str.appendSlice(allocator, num_str);
        }

        const result = try std.fmt.allocPrint(
            allocator,
            "Rolling {s}:\nIndividual rolls: [{s}]\nTotal: {d}",
            .{ self.notation, rolls_str.items, self.total }
        );
        rolls_str.deinit(allocator);

        return result;
    }
};

test "parse valid dice notation" {
    const roll = try DiceRoll.parse("3d8");
    try std.testing.expectEqual(@as(u32, 3), roll.count);
    try std.testing.expectEqual(@as(u32, 8), roll.sides);
}

test "parse single die" {
    const roll = try DiceRoll.parse("1d20");
    try std.testing.expectEqual(@as(u32, 1), roll.count);
    try std.testing.expectEqual(@as(u32, 20), roll.sides);
}

test "parse invalid notation" {
    try std.testing.expectError(error.InvalidNotation, DiceRoll.parse("invalid"));
    try std.testing.expectError(error.InvalidNotation, DiceRoll.parse("d8"));
    try std.testing.expectError(error.InvalidNotation, DiceRoll.parse("3d"));
    try std.testing.expectError(error.InvalidCount, DiceRoll.parse("xd8"));
    try std.testing.expectError(error.InvalidSides, DiceRoll.parse("3dy"));
}

test "validate limits" {
    try std.testing.expectError(error.InvalidCount, DiceRoll.parse("0d6"));
    try std.testing.expectError(error.InvalidCount, DiceRoll.parse("101d6"));
    try std.testing.expectError(error.InvalidSides, DiceRoll.parse("3d0"));
    try std.testing.expectError(error.InvalidSides, DiceRoll.parse("3d1001"));
}

test "roll dice" {
    const roll = try DiceRoll.parse("3d6");
    var prng = std.Random.DefaultPrng.init(12345);
    const random = prng.random();

    const result = try roll.roll(random);
    try std.testing.expect(result >= 3 and result <= 18);
}

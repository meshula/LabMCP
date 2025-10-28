  Wrinkles Zig Codebase Style Guide

  File Organization and Documentation

  File-level Documentation (doc comments):
  - Use //! for top-level file documentation
  - Include a brief description of the module's purpose
  - Add detailed implementation notes when relevant
  - Example from bezier_curve.zig:1-37:
  //! Bezier Curve implementation
  //!
  //! A sequence of right met 2d Bezier curve segments...

  Imports:
  - Standard library first, then external, then internal imports
  - Group related imports together
  - Import specific items when clear (e.g., const expectEqual = std.testing.expectEqual)
  - Example from bezier_curve.zig:39-50

  Naming Conventions

  Types:
  - Struct/enum names: PascalCase (e.g., AffineTransform1D, ContinuousInterval)
  - Type aliases follow same convention
  - Constants are SCREAMING_SNAKE_CASE (e.g., EMPTY_REF, ZERO, IDENTITY)
  - Public associated types use PascalCase (e.g., ProjectionAlgorithms)

  Functions and Variables:
  - Functions: snake_case (e.g., output_at_input, project_instantaneous_cc)
  - Parameters: snake_case (e.g., input_ord, allocator)
  - Local variables: snake_case
  - Module-level variables: snake_case (e.g., u_val_of_midpoint, project_algo)

  Conventions:
  - Type parameters indicated by generic constraints, not Hungarian notation
  - _ prefix for internal/helper functions (e.g., _is_between, _is_approximately_linear)
  - Suffixes like _s indicate specific spaces/units (e.g., bounds_s for seconds)

  Function Declaration Style

  Basic Structure:
  pub fn function_name(
      self: @This(),
      allocator: std.mem.Allocator,
      parameter: Type,
  ) !ReturnType 
  {
      // implementation
  }

  Multiline Parameters:
  - One parameter per line when there are many parameters
  - Align closing parenthesis with function name
  - Return type on same line as closing paren

  Return Types:
  - Error unions: !Type or error{Specific}!Type
  - Always specify error set when practical
  - Use void explicitly for procedures
  - Example from mapping.zig:57-70:
  pub fn project_instantaneous_cc(
      self: @This(),
      input_ord: anytype,
  ) opentime.ProjectionResult
  {
      return switch (self) {
          inline else => |m| m.project_instantaneous_cc(/*...*/),
      };
  }

  Struct Definitions

  Field Declaration:
  - Public fields use simple assignment
  - Include types explicitly
  - Default values after =
  - Constants inline or as associated consts
  - Example from interval.zig:10-26:
  pub const ContinuousInterval = struct {
      /// the start ordinate of the interval, inclusive
      start: ordinate.Ordinate = ordinate.Ordinate.ZERO,

      /// the end ordinate of the interval, exclusive
      end: ordinate.Ordinate = ordinate.Ordinate.INF,

      pub const INF : ContinuousInterval = .{/*...*/};
      pub const ZERO : ContinuousInterval = .{/*...*/};

      // methods follow...
  };

  Method Order:
  1. Associated constants
  2. init functions (constructors)
  3. Core methods
  4. Utility methods
  5. deinit/cleanup functions
  6. format (for custom formatting)

  Control Flow and Expressions

  Switch Statements:
  - Use inline else for type-generic handling
  - Tag-dispatching preferred over runtime branching
  - Example from mapping.zig:613-654:
  return switch (b2c_trimmed) {
      .affine => |b2c_aff| switch (a2b_trimmed) {
          .affine => |a2b_aff| join_aff_aff(/*...*/),
          .linear => |a2b_lin| (try join_lin_aff(/*...*/))),
          inline else => unreachable,
      },
      .linear => |b2c_lin| switch (a2b_trimmed) {
          .affine => |a2b_aff| (try join_aff_lin(/*...*/)),
          .linear => |a2b_lin| (try join_lin_lin(/*...*/)),
          inline else => unreachable,
      },
      inline else => unreachable,
  };

  If Statements:
  - Opening brace on same line
  - Use optional unwrapping where appropriate
  - Example from transform.zig:27-34:
  pub fn applied_to_ordinate(
      self: @This(),
      ord: ordinate.Ordinate,
  ) ordinate.Ordinate
  {
      return comath_wrapper.eval(
          "ord * scale + offset",
          .{ .ord = ord, .scale = self.scale, .offset = self.offset }
      );
  }

  Loops:
  - For loops: use capture syntax for indices when needed
  - While loops: condition on same line
  - Example from bezier_curve.zig:986-989:
  for (crv.knots[0..knots-1], crv.knots[1..])
      |knot, next_knot|
  {
      try result.append(Segment.init_from_start_end(knot, next_knot));
  }

  Error Handling

  Error Sets:
  - Define explicitly when practical
  - Group in associated const when part of interface
  - Example from mapping.zig:302:
  // @{ Errors
  pub const ProjectionError = error { OutOfBounds };
  // @}

  Error Propagation:
  - Use try for immediate propagation
  - Use errdefer for cleanup on error paths
  - Pattern from multiple files:
  const result = try allocator.dupe(Type, slice);
  errdefer allocator.free(result);

  Memory Management

  Uses the "unmanaged" style of allocators whenever possible, including using
  the unmanaged variants of stdlib structures (ie `ArrayList`, etc).

  Reasons:

  - smaller objects (don't need to carry the pointer to the allocator)
  - passing the allocator into a function is a clear signal to the caller that
    this function will allocate memory
  - explicit allocators makes it very easy to create arenas at different points
    in the function call stack to control memory lifetimes and performance

  Implementation details:

  - Pass allocator as first parameter after self
  - Always defer cleanup immediately after allocation
  - Use arena allocators for complex temporary allocations
  - Write functions assuming that they are not in arena IE that they clean up
    after themselves.
  - when using an arena in a function, name allocator argument
    `allocator_parent`  and the arena allocator `allocator_arena` to
    distinguish the allocators and signal allocations that are meant to live in
    the parent scope.
  - For executables, use the `std.heap.DebugAllocator` when building in debug
    mode and `std.heap.smp_allocator` when in release mode, IE:

  ```zig
      // use the debug allocator in debug builds, otherwise use smp
      const allocator = (
          if (builtin.mode == .Debug) alloc: {
              var da = std.heap.DebugAllocator(.{}){};
              break :alloc da.allocator();
          } else std.heap.smp_allocator
      );
  ```

  - Example from topology/root.zig:1119-1122:
  ```zig
  pub fn join(
      parent_allocator: std.mem.Allocator,
      topologies: struct{
          a2b: Topology,
          b2c: Topology,
      },
  ) !Topology
  {
      var arena = std.heap.ArenaAllocator.init(parent_allocator);
      defer arena.deinit();
      const allocator = arena.allocator();
      // ... work with allocator
  }
  ```

  Ownership:
  - Functions that allocate typically return !Type and transfer ownership
  - init functions duplicate and own
  - clone functions create owned copies
  - deinit functions free owned memory

  Testing

  Test Naming:
  - Descriptive names: "ModuleName: what is being tested"
  - Example from interval.zig:126:
  test "ContinuousInterval: is_infinite"
  {
      // test body
  }

  Test Structure:
  - Use std.testing.allocator for allocations
  - Defer cleanup immediately after allocation
  - Use struct-based test fixtures for multiple similar tests
  - Example from interval.zig:155-196:
  test "ContinuousInterval: extend"
  {
      const TestStruct = struct {
          fst: ContinuousInterval,
          snd: ContinuousInterval,
          res: ContinuousInterval,
      };
      const tests = [_]TestStruct{
          // test cases...
      };

      for (tests) |t| {
          const measured = extend(t.fst, t.snd);
          // assertions...
      }
  }

  Assertions:
  - Use specific assertion functions when available
  - Custom equality testers for domain types (e.g., expectOrdinateEqual)
  - errdefer for debug output on failure

  Comments and Documentation

  Function Documentation:
  - Use /// for public API documentation
  - Document parameters, return values, and behavior
  - Note any assumptions or preconditions
  - Example from transform.zig:80-98:
  /// Return the inverse of this transform.
  ///    ** assumes that scale is non-zero **
  ///
  /// Because the AffineTransform1D is a 2x2 matrix of the form:
  ///     | scale offset |
  ///     |   0     1    |
  ///
  /// The inverse is:
  ///     | 1/scale -offset/scale |
  ///     |   0           1       |

  Inline Comments:
  - Explain non-obvious logic
  - Mark TODOs with @TODO: prefix
  - Use section markers for organization: // @{ SectionName } and // @}
  - Example from various files:
  // @TODO: this function should compute and preserve the derivatives

  Type Expressions and Inference

  Type Specification:
  - Explicit types for struct fields
  - Inference OK for local variables with clear initialization
  - Use @This() for self-referential types
  - Example from bezier_curve.zig:588-596:
  pub inline fn eval_at(
      self: @This(),
      unorm: anytype,
  ) control_point.ControlPoint
  {
      return switch (@TypeOf(unorm)) {
          opentime.Ordinate => self.eval_at_ord(unorm),
          else => self.eval_at_ord(opentime.Ordinate.init(unorm)),
      };
  }

  Comptime and Generics

  Generic Functions:
  - Use anytype for flexible parameters
  - Runtime type switching with @TypeOf()
  - inline for for compile-time iteration
  - Example from bezier_curve.zig:348-356:
  inline for ([_]opentime.Ordinate.BaseType{2.1, 2.2, 2.3, 2.5, 2.7}) 
             |coord| 
  {
      try opentime.expectOrdinateEqual(
          coord,
          seg.output_at_input(coord)
      );
  }

  Formatting Preferences

  Indentation:
  - 4 spaces per level
  - Align related items vertically when helpful
  - Example from schema.zig:66-78:
  pub const ParameterVarying = struct {
      domain: Domain,
      mapping: topology_m.Topology,

      pub fn parameter(
          self: *const @This()
      ) Parameter
      {
          return .{
              .value = self,
          };
      }
  };

  Line Length:
  - No strict limit, but prefer wrapping long lines
  - Break at logical boundaries
  - Align continuation lines meaningfully

  Whitespace:
  - Blank line between major sections
  - No blank line between related field declarations
  - Space after keywords, around operators
  - No space inside parens/brackets

  Braces:
  - Opening brace on same line for control flow
  - Closing brace aligned with start of statement
  - Struct literals can be inline for small structs

  ---
  This style guide reflects the consistent patterns found throughout the wrinkles codebase.

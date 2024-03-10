#pragma once

#include <mouse_logic/std.hpp>

enum class Direction : uint8_t {
  NORTH = 0,
  EAST  = 1,
  SOUTH = 2,
  WEST  = 3
};

static constexpr Direction opposite(Direction dir) {
  return static_cast<Direction>((static_cast<uint8_t>(dir) + 2) % 4);
}

static constexpr Direction left_of(Direction dir) {
  return static_cast<Direction>((static_cast<uint8_t>(dir) + 3) % 4);
}

static constexpr Direction right_of(Direction dir) {
  return static_cast<Direction>((static_cast<uint8_t>(dir) + 1) % 4);
}

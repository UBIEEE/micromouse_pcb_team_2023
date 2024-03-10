#pragma once

#include <mouse_logic/std.hpp>

#define MAZE_WIDTH 16

class Coordinate {
  uint8_t m_index;

public:
  constexpr Coordinate(uint8_t x, uint8_t y)
    : m_index(y * MAZE_WIDTH + x) {

    assert(x < MAZE_WIDTH);
    assert(y < MAZE_WIDTH);
  }

  constexpr Coordinate(std::pair<uint8_t, uint8_t> coord)
    : Coordinate(coord.first, coord.second) {}

  explicit constexpr Coordinate(uint8_t index)
    : m_index(index) {}

  operator uint8_t() const { return m_index; }
  operator std::pair<uint8_t, uint8_t>() const { return to_pair(); }

  constexpr uint8_t x() const { return m_index % MAZE_WIDTH; }
  constexpr uint8_t y() const { return m_index / MAZE_WIDTH; }

  constexpr std::pair<uint8_t, uint8_t> to_pair() const { return {x(), y()}; }
};

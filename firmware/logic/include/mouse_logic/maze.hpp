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

//
// Represents a cell in the maze.
// Contains data for the walls and whether the cell has been visited.
//
// 1 byte in size.
//
class Cell {
  union {
    uint8_t m_data;
    struct {
      const uint8_t m_north : 1;
      const uint8_t m_east : 1;
      const uint8_t m_south : 1;
      const uint8_t m_west : 1;
      const uint8_t m_visited : 4;
    };
  };

public:
  Cell()
    : m_data(0x00) {}

  Cell(bool north, bool east, bool south, bool west)
    : m_north(north),
      m_east(east),
      m_south(south),
      m_west(west),
      m_visited(0) {}

  ~Cell() = default;

  bool operator==(const Cell& other) const { return m_data == other.m_data; }
  bool operator!=(const Cell& other) const { return m_data != other.m_data; }

  bool is_wall(Direction dir) const {
    const uint8_t mask = 1 << static_cast<uint8_t>(dir);
    return (m_data & mask) != 0;
  }

  bool is_exit(Direction dir) const {
    const uint8_t mask = 1 << static_cast<uint8_t>(dir);
    return (m_data & mask) == 0;
  }

  void set_wall(Direction dir, bool value = true) {
    const uint8_t mask = 1 << static_cast<uint8_t>(dir);

    // Clear the bit
    m_data &= ~mask;
    // Set the bit
    if (value) m_data |= mask;
  }

  bool is_visited() const { return m_visited != 0; }

  void set_visited() noexcept { m_data |= 0xF0; }
};

static_assert(sizeof(Cell) == sizeof(uint8_t));

#define MAZE_WIDTH 16
#define MAZE_CELLS (MAZE_WIDTH * MAZE_WIDTH)

class MazeCoordinate {
  uint8_t m_index;

public:
  constexpr MazeCoordinate(uint8_t x, uint8_t y)
    : m_index(y * MAZE_WIDTH + x) {
    assert(x < MAZE_WIDTH);
    assert(y < MAZE_WIDTH);
  }

  constexpr MazeCoordinate(std::pair<uint8_t, uint8_t> coord)
    : MazeCoordinate(coord.first, coord.second) {}

  operator uint8_t() const { return m_index; }

  uint8_t x() const { return m_index % MAZE_WIDTH; }
  uint8_t y() const { return m_index / MAZE_WIDTH; }
};

//
// Represents the maze.
//
// +x is east
// +y is north
//
//    · · · · ·
//    · · · · ·
//    · · t · ·
//    · · · · ·
//    r · · · s
//
// _r_ and _s_ are start cells. The mouse starts facing north.
// _t_ is the target cell.
//
class Maze {
  Cell m_cells[MAZE_CELLS];
  uint8_t m_cell_costs[MAZE_CELLS];

public:
  // Center four cells.
  inline static constexpr MazeCoordinate GOAL_ENDPOINTS[] = {
      {7, 7}, {7, 8}, {8, 7}, {8, 8}};

  // Southwest corner cell (goal is to the right).
  inline static constexpr MazeCoordinate WEST_START_ENDPOINTS[] = {{0, 0}};

  // Southeast corner cell (goal is to the left).
  inline static constexpr MazeCoordinate EAST_START_ENDPOINTS[] = {{15, 0}};

  // The two possible start locations for the mouse.
  enum class StartLocation {
    WEST_OF_GOAL,
    EAST_OF_GOAL
  };

  // Returns the start endpoints based on the start location.
  static std::span<const MazeCoordinate>
  get_start_endpoints(StartLocation start_location) {

    switch (start_location) {
      using enum StartLocation;
    case WEST_OF_GOAL:
      return WEST_START_ENDPOINTS;
    case EAST_OF_GOAL:
      return EAST_START_ENDPOINTS;
    }
  }

  Maze();
  ~Maze() = default;

  bool operator==(const Maze& other) const {
    return std::memcmp(m_cells, other.m_cells, MAZE_CELLS) == 0;
  }
  bool operator!=(const Maze& other) const { return !(*this == other); }

  // Zeroes out the maze and places boundaries. This does not need to be called
  // initially, just when the maze needs to be reset after being used.
  void reset();

  // Places a wall bounding the start cell based on the start location.
  void init_start_cell(StartLocation start_location);

  bool cell_is_wall(MazeCoordinate coord, Direction dir) const {
    return m_cells[coord].is_wall(dir);
  }
  void cell_set_wall(MazeCoordinate coord, Direction dir, bool present = true);

  bool cell_is_visited(MazeCoordinate coord) const {
    return m_cells[coord].is_visited();
  }
  void cell_set_visited(MazeCoordinate coord) { m_cells[coord].set_visited(); }

  Cell& get_cell(MazeCoordinate coord) { return m_cells[coord]; }
  Cell& operator[](MazeCoordinate coord) { return m_cells[coord]; }
  // Returns nullptr if the neighbor is out of bounds.
  Cell* get_neighbor_cell(MazeCoordinate coord, Direction direction);

  uint8_t get_cell_cost(MazeCoordinate coord) const {
    return m_cell_costs[coord];
  }

  void set_cell_cost(MazeCoordinate coord, uint8_t cost) {
    m_cell_costs[coord] = cost;
  }

private:
  // Places walls around the perimeter of the maze.
  void init_boundaries();
};


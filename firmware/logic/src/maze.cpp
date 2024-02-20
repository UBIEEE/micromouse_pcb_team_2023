#include <mouse_logic/maze.hpp>

Maze::Maze() { init_boundaries(); }

void Maze::reset() {
  std::memset(m_cells, 0, sizeof(m_cells));
  init_boundaries();
}

void Maze::init_start_cell(StartLocation start_location) {
  using enum Direction;

  if (start_location == StartLocation::WEST_OF_GOAL) {
    m_cells[0].set_wall(EAST);
  } else {
    m_cells[15].set_wall(WEST);
  }
}

void Maze::init_boundaries() {
  using enum Direction;

  for (uint8_t i = 0; i < MAZE_WIDTH; ++i) {
    // (0,0) -> (15,0) have south wall.
    m_cells[i].set_wall(SOUTH);
    // (0,15) -> (15,15) have north wall.
    m_cells[i + (MAZE_WIDTH * (MAZE_WIDTH - 1))].set_wall(NORTH);
    // (0,0) -> (0,15) have west wall.
    m_cells[i * MAZE_WIDTH].set_wall(WEST);
    // (15,0) -> (15,15) have east wall.
    m_cells[(i * MAZE_WIDTH) + (MAZE_WIDTH - 1)].set_wall(EAST);
  }
}

void Maze::cell_set_wall(MazeCoordinate coord, Direction dir, bool present) {
  m_cells[coord].set_wall(dir, present);

  Cell* neighbor = get_neighbor_cell(coord, dir);
  if (neighbor) {
    neighbor->set_wall(opposite(dir), present);
  }
}

Cell* Maze::get_neighbor_cell(MazeCoordinate coord, Direction direction) {
  int8_t x = coord.x();
  int8_t y = coord.y();

  switch (direction) {
    using enum Direction;
  case NORTH:
    ++y;
    break;
  case EAST:
    ++x;
    break;
  case SOUTH:
    --y;
    break;
  case WEST:
    --x;
    break;
  }

  if (x < 0 || x >= MAZE_WIDTH || y < 0 || y >= MAZE_WIDTH) return nullptr;

  return &m_cells[MazeCoordinate(x, y)];
}


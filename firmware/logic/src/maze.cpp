#include <mouse_logic/maze.hpp>

const MazeCoordinate MAZE_START = {0, 0};
const MazeCoordinate MAZE_GOAL  = {14, 14};

Maze::Maze() {
  std::memset(costs, 0, sizeof(costs));
  std::memset(walls, 0, sizeof(walls));

  // TODO: Initialize boundaries.
  // TODO: Start cell walls
}

void Maze::print() {
  // TODO: Print
}


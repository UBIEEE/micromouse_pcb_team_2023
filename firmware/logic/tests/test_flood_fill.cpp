#include <gtest/gtest.h>
#include <mouse_logic/maze.hpp>
#include <mouse_logic/flood_fill.hpp>

TEST(FloodFill, Test1) {
  Maze maze;
  FloodFillSolver solver(maze);
}


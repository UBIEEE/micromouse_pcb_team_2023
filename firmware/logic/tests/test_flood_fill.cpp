#include <gtest/gtest.h>
#include <mouse_logic/maze/maze.hpp>
#include <mouse_logic/solvers/flood_fill.hpp>

TEST(FloodFill, Test1) {
  Maze maze;
  FloodFillSolver solver(maze);
}


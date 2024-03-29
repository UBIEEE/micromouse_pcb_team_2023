#pragma once

#include <mouse_logic/maze/maze.hpp>
#include <mouse_logic/std.hpp>

class FloodFillSolver {
  Maze& m_maze;

public:
  FloodFillSolver(Maze& maze)
    : m_maze(maze) {}

  void solve(std::span<Coordinate> endpoints);
};


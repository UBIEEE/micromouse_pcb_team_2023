#pragma once

#include <mouse_logic/maze.hpp>
#include <mouse_logic/std.hpp>

class Solver {
protected:
  Maze& m_maze;

public:
  Solver(Maze& maze)
    : m_maze(maze) {}

  virtual ~Solver() = default;

  virtual void solve(MazeCoordinate target) = 0;
};

class FloodFillSolver : public Solver {
public:
  FloodFillSolver(Maze& maze)
    : Solver(maze) {}

  ~FloodFillSolver() override = default;

  void solve(MazeCoordinate target) override;
};


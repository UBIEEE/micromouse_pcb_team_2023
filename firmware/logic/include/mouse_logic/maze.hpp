#pragma once

#include <mouse_logic/std.hpp>

struct MazeCoordinate {
  uint8_t x;
  uint8_t y;
};

extern const MazeCoordinate MAZE_START; // 0,0
extern const MazeCoordinate MAZE_GOAL;  // 14,14

#define MAZE_WIDTH 16

enum {
  CELL_NORTH   = 0b0000'0001,
  CELL_EAST    = 0b0000'0010,
  CELL_SOUTH   = 0b0000'0100,
  CELL_WEST    = 0b0000'1000,
  CELL_VISITED = 0b1111'0000,
};

struct Maze {
  uint8_t costs[MAZE_WIDTH][MAZE_WIDTH];
  uint8_t walls[MAZE_WIDTH][MAZE_WIDTH];

  Maze();

  void print();
};


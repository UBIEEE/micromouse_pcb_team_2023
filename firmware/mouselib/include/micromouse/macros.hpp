#pragma once

#include <micromouse/picosdk.hpp>

// Prints the current source location and a message, then halts execution.
#define FATAL_ERROR(TAG, FMT, ...)                                             \
  pico::panic(TAG " [%s:%s:%d] " FMT "\n", __FILE__, __func__, __LINE__,       \
              ##__VA_ARGS__)

#define TAG_INVALID_ARGS "[INVALID ARGS]"
#define TAG_SPI_ERROR    "[SPI ERROR]"

// Fatal error for invalid arguments.
#define INVALID_ARGS() FATAL_ERROR(TAG_INVALID_ARGS, "")


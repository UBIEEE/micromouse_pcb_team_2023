#pragma once

//
// C standard library
// Everything here is fair game performance-wise.
//
#include <climits>
#include <cmath>
#include <cstdarg>
#include <cstdbool>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

//
// Select C++ standard library stuff.
//
#include <chrono> // Only for duration literals!
#include <mutex>  // NOT for std::mutex, only for std::lock_guard!
#include <algorithm>

using namespace std::chrono_literals;


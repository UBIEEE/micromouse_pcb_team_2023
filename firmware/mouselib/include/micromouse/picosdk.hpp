#pragma once

#include <micromouse/std.hpp>

typedef struct spi_inst spi_inst_t;
typedef struct spi_hw spi_hw_t;

namespace pico {

#include <hardware/irq.h>
#include <hardware/pwm.h>
#include <hardware/spi.h>
#include <pico/multicore.h>
#include <pico/mutex.h>
#include <pico/stdlib.h>

//
// Wrapper around the Pico SDK's mutex_t type.
// Conforms to BasicLockable, so can be used with std::lock_guard.
//
class Mutex {
  mutex_t m_mutex;

public:
  Mutex() { mutex_init(&m_mutex); }

  ~Mutex() {
    // The Pico SDK doesn't provide a destroy function D:
    // Let's just never destroy mutexes.... that should be fine....
    panic("Mutex destructor called D:\n");
  }

  void lock() { mutex_enter_blocking(&m_mutex); }
  void unlock() { mutex_exit(&m_mutex); }
};

//
// Wrapper around the Pico SDK's absolute_time_t type.
//
class AbsoluteTime {
  absolute_time_t m_time;

  /*implicit*/ AbsoluteTime(absolute_time_t time)
    : m_time(time) {}

public:
  static AbsoluteTime now() { return get_absolute_time(); }

  // us
  AbsoluteTime operator+(const std::chrono::microseconds& duration) const {
    return delayed_by_us(m_time, duration.count());
  }
  AbsoluteTime operator-(const std::chrono::microseconds& duration) const {
    return delayed_by_us(m_time, -duration.count());
  }
  AbsoluteTime& operator+=(const std::chrono::microseconds& duration) {
    m_time = delayed_by_us(m_time, duration.count());
    return *this;
  }
  AbsoluteTime& operator-=(const std::chrono::microseconds& duration) {
    m_time = delayed_by_us(m_time, -duration.count());
    return *this;
  }

  // ms
  AbsoluteTime operator+(const std::chrono::milliseconds& duration) const {
    return delayed_by_ms(m_time, duration.count());
  }
  AbsoluteTime operator-(const std::chrono::milliseconds& duration) const {
    return delayed_by_ms(m_time, -duration.count());
  }
  AbsoluteTime& operator+=(const std::chrono::milliseconds& duration) {
    m_time = delayed_by_ms(m_time, duration.count());
    return *this;
  }
  AbsoluteTime& operator-=(const std::chrono::milliseconds& duration) {
    m_time = delayed_by_ms(m_time, -duration.count());
    return *this;
  }

  // s
  AbsoluteTime operator+(const std::chrono::seconds& duration) const {
    return delayed_by_ms(m_time, duration.count() * 1000);
  }
  AbsoluteTime operator-(const std::chrono::seconds& duration) const {
    return delayed_by_ms(m_time, -duration.count() * 1000);
  }
  AbsoluteTime& operator+=(const std::chrono::seconds& duration) {
    m_time = delayed_by_ms(m_time, duration.count() * 1000);
    return *this;
  }
  AbsoluteTime& operator-=(const std::chrono::seconds& duration) {
    m_time = delayed_by_ms(m_time, -duration.count() * 1000);
    return *this;
  }

  // comparison
  bool operator==(const AbsoluteTime& other) const {
    return to_us_since_boot(m_time) == to_us_since_boot(other.m_time);
  }
  bool operator!=(const AbsoluteTime& other) const {
    return !(*this == other);
  }
  bool operator<(const AbsoluteTime& other) const {
    return to_us_since_boot(m_time) < to_us_since_boot(other.m_time);
  }
  bool operator<=(const AbsoluteTime& other) const {
    return to_us_since_boot(m_time) <= to_us_since_boot(other.m_time);
  }
  bool operator>(const AbsoluteTime& other) const {
    return to_us_since_boot(m_time) > to_us_since_boot(other.m_time);
  }
  bool operator>=(const AbsoluteTime& other) const {
    return to_us_since_boot(m_time) >= to_us_since_boot(other.m_time);
  }

  // conversion
  operator absolute_time_t() const { return m_time; }
};

} // namespace pico


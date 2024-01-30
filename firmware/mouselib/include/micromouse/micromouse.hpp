#pragma once

#include <micromouse/picosdk.hpp>
#include <micromouse/std.hpp>

#include <micromouse/hardware/ma730.hpp>
#include <micromouse/hardware/drv8835.hpp>
#include <micromouse/hardware/icm20602.hpp>

class MicroMouse {
  pico::spi_inst_t* m_spi;

  hardware::MA730 m_enc_left;
  hardware::MA730 m_enc_right;
  hardware::DRV8835 m_motors;
  hardware::ICM20602 m_imu;

  struct SensorData {
    float enc_left_dist_m  = 0.f;
    float enc_right_dist_m = 0.f;

    float enc_left_vel_mps  = 0.f;
    float enc_right_vel_mps = 0.f;

    float enc_left_angle_deg;
    float enc_right_angle_deg;

    float imu_yaw_angle_deg;
    float imu_yaw_rate_dps = 0.f;
  };

  SensorData m_sensor_data;
  pico::Mutex m_sensor_data_mutex;

  MicroMouse();

public:
  //
  // Initialize the spi instance used by the hardware.
  // Called by the constructor.
  //
  static pico::spi_inst_t* init_spi();

  //
  // Singleton!
  // It'll only be initialized when get() is called for the first time.
  //
  static MicroMouse& get() {
    static MicroMouse instance;
    return instance;
  }

  MicroMouse(const MicroMouse&)            = delete;
  MicroMouse(MicroMouse&&)                 = delete;
  MicroMouse& operator=(const MicroMouse&) = delete;
  MicroMouse& operator=(MicroMouse&&)      = delete;

  //
  // Launches core 1, then runs the primary loop on core 0.
  // This function never returns.
  //
  [[noreturn]] void run();

private:
  [[noreturn]] void core0_entry(); // Primary
  [[noreturn]] void core1_entry(); // Secondary

  using LoopFunc = void (MicroMouse::*)();

  // Calls func every period forever.
  [[noreturn]] void loop(LoopFunc func, std::chrono::milliseconds period);

  // Core 0: Handles logic and control.
  void primary_fixed_update();

  // Core 1: Handles sensor reading.
  void secondary_fixed_update();

  //
  // Calculates the distance traveled since the last update based on the encoder's
  // current angle its last angle.
  // The angles should be represented in degrees, [0, 360].
  //
  static float calc_encoder_delta_distance(float angle, float last_angle);
};


#pragma once

#include <mouse_control/picosdk.hpp>
#include <mouse_control/std.hpp>

namespace hardware {

//
// Datasheet:
// https://invensense.tdk.com/wp-content/uploads/2016/10/DS-000176-ICM-20602-v1.0.pdf
//
class ICM20602 {
  pico::spi_inst_t* m_spi;
  const uint8_t m_cs_pin;

public:
  static constexpr float INVALID_VALUE = -1.f;

  //
  // A_DLPF_CFG in the ACCEL_CONFIG2 register, except for BYPASS cases.
  //
  enum class AccelDLPF : uint8_t {
    _218_1_HZ = 0,
    _99_HZ    = 2,
    _44_8_HZ  = 3,
    _21_2_HZ  = 4,
    _10_2_HZ  = 5,
    _5_1_HZ   = 6,
    _420_HZ   = 7,
    _BYPASS_1046_HZ, // No filter
  };

  //
  // ACCEL_FS_SEL in the ACCEL_CONFIG register.
  //
  enum class AccelRange : uint8_t {
    _2_G  = 0,
    _4_G  = 1,
    _8_G  = 2,
    _16_G = 3,
  };

  //
  // DLPF_CFG in the CONFIG register, except for BYPASS cases.
  //
  enum class GyroDLPF : uint8_t {
    _250_HZ  = 0,
    _176_HZ  = 1,
    _92_HZ   = 2,
    _41_HZ   = 3,
    _20_HZ   = 4,
    _10_HZ   = 5,
    _5_HZ    = 6,
    _3281_HZ = 7,
    _BYPASS_3281_HZ, // No filter
    _BYPASS_8173_HZ, // No filter
  };

  //
  // FS_SEL in the GYRO_CONFIG register.
  //
  // Note: DPS refers to degrees per second.
  //
  enum class GyroRange : uint8_t {
    _250_DPS  = 0,
    _500_DPS  = 1,
    _1000_DPS = 2,
    _2000_DPS = 3,
  };

  //
  // Config for the IMU.
  //
  struct Config {
    AccelDLPF accel_dlpf   = AccelDLPF::_10_2_HZ;
    AccelRange accel_range = AccelRange::_4_G;
    bool accel_fifo        = false;

    GyroDLPF gyro_dlpf   = GyroDLPF::_10_HZ;
    GyroRange gyro_range = GyroRange::_250_DPS;
    bool gyro_fifo       = false;

    uint8_t sample_rate_divider = 100;
  };

  //
  // Initializes a ICM20602 IMU.
  // The program will halt if the device could not be initialized.
  //
  // Important: The SPI instance must be initialized prior to calling this!
  //
  ICM20602(pico::spi_inst_t* spi, uint8_t cs_pin, const Config& config);

  struct Data {
    float x;
    float y;
    float z;
  };

  enum class Axis {
    X     = 0,
    Y     = 2,
    Z     = 4,
    PITCH = X,
    ROLL  = Y,
    YAW   = Z,
  };

  Data read_gyro() const;
  float read_gyro_axis(Axis axis) const;

  Data read_accel() const;
  float read_accel_axis(Axis axis) const;

private:
  Config m_config;

private:
  void read_register(uint8_t reg, uint8_t* buf, uint8_t len) const;
  void write_register(uint8_t reg, const uint8_t* buf, uint8_t len) const;

  struct RawData {
    int16_t x;
    int16_t y;
    int16_t z;
  };

  RawData read_axes_raw(uint8_t addr) const;
  float read_axis_raw(uint8_t addr) const;

  static float get_accel_range(AccelRange range) {
    const float ranges[] = {2.f, 4.f, 8.f, 16.f};
    return ranges[static_cast<uint8_t>(range)];
  }

  static float get_gyro_range(GyroRange range) {
    const float ranges[] = {250.f, 500.f, 1000.f, 2000.f};
    return ranges[static_cast<uint8_t>(range)];
  }

  static float convert_gyro_reading(int16_t value, GyroRange range) {
    return (static_cast<float>(value) / INT16_MAX) * get_gyro_range(range);
  }

  static float convert_accel_reading(int16_t value, AccelRange range) {
    return (static_cast<float>(value) / INT16_MAX) * get_accel_range(range);
  }
};

} // namespace hardware


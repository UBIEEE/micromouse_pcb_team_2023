#include <mouse_control/hardware/icm20602.hpp>

#include <mouse_control/constants.hpp>
#include <mouse_control/macros.hpp>

using namespace hardware;

#define REG_XG_OFFS_TC_H       0x04
#define REG_XG_OFFS_TC_L       0x05
#define REG_YG_OFFS_TC_H       0x07
#define REG_YG_OFFS_TC_L       0x08
#define REG_ZG_OFFS_TC_H       0x0A
#define REG_ZG_OFFS_TC_L       0x0B
#define REG_SELF_TEST_X_ACCEL  0x0D
#define REG_SELF_TEST_Y_ACCEL  0x0E
#define REG_SELF_TEST_Z_ACCEL  0x0F
#define REG_XG_OFFS_USRH       0x13
#define REG_XG_OFFS_USRL       0x14
#define REG_YG_OFFS_USRH       0x15
#define REG_YG_OFFS_USRL       0x16
#define REG_ZG_OFFS_USRH       0x17
#define REG_ZG_OFFS_USRL       0x18
#define REG_SMPLRT_DIV         0x19
#define REG_CONFIG             0x1A
#define REG_GYRO_CONFIG        0x1B
#define REG_ACCEL_CONFIG       0x1C
#define REG_ACCEL_CONFIG_2     0x1D
#define REG_LP_MODE_CFG        0x1E
#define REG_ACCEL_WOM_X_THR    0x20
#define REG_ACCEL_WOM_Y_THR    0x21
#define REG_ACCEL_WOM_Z_THR    0x22
#define REG_FIFO_EN            0x23
#define REG_FSYNC_INT          0x36
#define REG_INT_PIN_CFG        0x37
#define REG_INT_ENABLE         0x38
#define REG_FIFO_WM_INT_STATUS 0x39
#define REG_INT_STATUS         0x3A
#define REG_ACCEL_XOUT_H       0x3B
#define REG_ACCEL_XOUT_L       0x3C
#define REG_ACCEL_YOUT_H       0x3D
#define REG_ACCEL_YOUT_L       0x3E
#define REG_ACCEL_ZOUT_H       0x3F
#define REG_ACCEL_ZOUT_L       0x40
#define REG_TEMP_OUT_H         0x41
#define REG_TEMP_OUT_L         0x42
#define REG_GYRO_XOUT_H        0x43
#define REG_GYRO_XOUT_L        0x44
#define REG_GYRO_YOUT_H        0x45
#define REG_GYRO_YOUT_L        0x46
#define REG_GYRO_ZOUT_H        0x47
#define REG_GYRO_ZOUT_L        0x48
#define REG_SELF_TEST_X_GYRO   0x50
#define REG_SELF_TEST_Y_GYRO   0x51
#define REG_SELF_TEST_Z_GYRO   0x52
#define REG_FIFO_WM_TH1        0x60
#define REG_FIFO_WM_TH2        0x61
#define REG_SIGNAL_PATH_RESET  0x68
#define REG_ACCEL_INTEL_CTRL   0x69
#define REG_USER_CTRL          0x6A
#define REG_PWR_MGMT_1         0x6B
#define REG_PWR_MGMT_2         0x6C
#define REG_I2C_IF             0x70
#define REG_FIFO_COUNTH        0x72
#define REG_FIFO_COUNTL        0x73
#define REG_FIFO_R_W           0x74
#define REG_WHO_AM_I           0x75
#define REG_XA_OFFSET_H        0x77
#define REG_XA_OFFSET_L        0x78
#define REG_YA_OFFSET_H        0x7A
#define REG_YA_OFFSET_L        0x7B
#define REG_ZA_OFFSET_H        0x7D
#define REG_ZA_OFFSET_L        0x7E

#define REG_WHO_AM_I_CONST 0X12

#define CS_IDLE   1
#define CS_ACTIVE 0

ICM20602::ICM20602(pico::spi_inst_t* spi, uint8_t cs_pin, const Config& config)
  : m_spi(spi),
    m_cs_pin(cs_pin),
    m_config(config) {

  if (!spi) INVALID_ARGS();

  // Initialize pins.
  pico::gpio_init(cs_pin);
  pico::gpio_set_dir(cs_pin, GPIO_OUT);
  pico::gpio_put(cs_pin, CS_IDLE);

  uint8_t tmp = 0x00;

  // Reset.
  tmp = 0x80;
  write_register(REG_PWR_MGMT_1, &tmp, 1);

  // TODO: Sleep 1000

  // Read something.
  read_register(REG_WHO_AM_I, &tmp, 1);

  if (tmp != REG_WHO_AM_I_CONST) {
    FATAL_ERROR(TAG_SPI_ERROR, "ICM20602 WHO_AM_I mismatch");
  }

  // Set clock to internal PLL.
  tmp = 0x01;
  write_register(REG_PWR_MGMT_1, &tmp, 1);

  // Place accel and gryo on standby.
  tmp = 0x3F;
  write_register(REG_PWR_MGMT_2, &tmp, 1);

  // Disable FIFO.
  tmp = 0x00;
  write_register(REG_USER_CTRL, &tmp, 1);

  // Enable accelerometer.
  if (AccelDLPF::_BYPASS_1046_HZ == m_config.accel_dlpf) {
    tmp = (1 << 3);
    write_register(REG_ACCEL_CONFIG_2, &tmp, 1);
  } else {
    tmp = static_cast<uint8_t>(m_config.accel_dlpf);
    write_register(REG_ACCEL_CONFIG_2, &tmp, 1);
  }

  tmp = (static_cast<uint8_t>(m_config.accel_range) << 2);
  write_register(REG_ACCEL_CONFIG, &tmp, 1);

  // Enable gyro.
  if (GyroDLPF::_BYPASS_3281_HZ == m_config.gyro_dlpf) {
    // Bypass DLPF.
    tmp = 0x00;
    write_register(REG_CONFIG, &tmp, 1);

    // Set range.
    tmp = (static_cast<uint8_t>(m_config.gyro_range) << 3) | 0x02;
    write_register(REG_GYRO_CONFIG, &tmp, 1);

  } else if (GyroDLPF::_BYPASS_8173_HZ == m_config.gyro_dlpf) {
    // Bypass DLPF.
    tmp = 0x00;
    write_register(REG_CONFIG, &tmp, 1);

    // Set range.
    tmp = (static_cast<uint8_t>(m_config.gyro_range) << 3) | 0x01;
    write_register(REG_GYRO_CONFIG, &tmp, 1);
  } else {
    // Configure DLPF.
    tmp = static_cast<uint8_t>(m_config.gyro_dlpf);
    write_register(REG_CONFIG, &tmp, 1);

    // Set range.
    tmp = static_cast<uint8_t>(m_config.gyro_range) << 3;
    write_register(REG_GYRO_CONFIG, &tmp, 1);
  }

  // No FIFO.
  tmp = static_cast<uint8_t>(m_config.accel_fifo) ? 0x08 : 0x00;
  tmp |= static_cast<uint8_t>(m_config.gyro_fifo) ? 0x10 : 0x00;
  write_register(REG_FIFO_EN, &tmp, 1);

  // Sample rate divider.
  tmp = (m_config.sample_rate_divider) ? (m_config.sample_rate_divider - 1) : 1;
  write_register(REG_SMPLRT_DIV, &tmp, 1);

  tmp = 0;
  write_register(REG_PWR_MGMT_2, &tmp, 1);
}

ICM20602::Data ICM20602::read_gyro() const {
  const RawData data = read_axes_raw(REG_GYRO_XOUT_H);

  return {
      .x = convert_gyro_reading(data.x, m_config.gyro_range),
      .y = convert_gyro_reading(data.y, m_config.gyro_range),
      .z = convert_gyro_reading(data.z, m_config.gyro_range),
  };
}

float ICM20602::read_gyro_axis(Axis axis) const {
  const int16_t raw = read_axis_raw(REG_GYRO_XOUT_H + (uint8_t)axis);

  return convert_gyro_reading(raw, m_config.gyro_range);
}

ICM20602::Data ICM20602::read_accel() const {
  const RawData data = read_axes_raw(REG_ACCEL_XOUT_H);

  return {
      .x = convert_accel_reading(data.x, m_config.accel_range),
      .y = convert_accel_reading(data.y, m_config.accel_range),
      .z = convert_accel_reading(data.z, m_config.accel_range),
  };
}

float ICM20602::read_accel_axis(Axis axis) const {
  const int16_t raw = read_axis_raw(REG_ACCEL_XOUT_H + (uint8_t)axis);

  return convert_accel_reading(raw, m_config.accel_range);
}

void ICM20602::read_register(uint8_t reg, uint8_t* buf, uint8_t len) const {
  if (!buf) INVALID_ARGS();

  const uint8_t cmd   = reg | 0x80; // MSB 1 for read.
  const uint8_t dummy = 0x00;

  pico::gpio_put(m_cs_pin, CS_ACTIVE);

  if (1 != pico::spi_write_blocking(m_spi, &cmd, 1)) {
    FATAL_ERROR(TAG_SPI_ERROR, "ICM20602 read_register() failed: "
                               "could not write command byte");
  }

  for (uint8_t i = 0; i < len; i++) {
    // Send dummy byte.
    if (1 != pico::spi_write_blocking(m_spi, &dummy, 1)) {
      FATAL_ERROR(TAG_SPI_ERROR, "ICM20602 read_register() failed: "
                                 "could not write dummy byte");
    }

    // Read byte.
    if (1 != pico::spi_read_blocking(m_spi, 0, &buf[i], 1)) {
      FATAL_ERROR(TAG_SPI_ERROR, "ICM20602 read_register() failed: "
                                 "could not read byte");
    }
  }

  pico::gpio_put(m_cs_pin, CS_IDLE);
}

void ICM20602::write_register(uint8_t reg, const uint8_t* buf,
                              uint8_t len) const {

  if (!buf) INVALID_ARGS();

  const uint8_t cmd = reg & 0x7F; // MSB 0 for write.

  pico::gpio_put(m_cs_pin, CS_ACTIVE);

  if (1 != pico::spi_write_blocking(m_spi, &cmd, 1)) {
    FATAL_ERROR(TAG_SPI_ERROR, "ICM20602 write_register() failed: "
                               "could not write command byte");
  }

  if (len != pico::spi_write_blocking(m_spi, buf, len)) {
    FATAL_ERROR(TAG_SPI_ERROR, "ICM20602 write_register() failed: "
                               "could not write data");
  }

  pico::gpio_put(m_cs_pin, CS_IDLE);
}

ICM20602::RawData ICM20602::read_axes_raw(uint8_t addr) const {
  uint8_t buf[6] = {0};

  read_register(addr, buf, 6);

  const int16_t x = int16_t((buf[0]) << 8) | buf[1];
  const int16_t y = int16_t((buf[2]) << 8) | buf[3];
  const int16_t z = int16_t((buf[4]) << 8) | buf[5];

  return {x, y, z};
}

float ICM20602::read_axis_raw(uint8_t addr) const {
  uint8_t buf[2];

  read_register(addr, buf, 2);

  return (int16_t(buf[0]) << 8) | buf[1];
}


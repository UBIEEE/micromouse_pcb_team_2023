#include <micromouse/hardware/icm20602.h>

#include <micromouse/constants.h>
#include <micromouse/diagnostics.h>

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

static result_t read_register(icm20602_dev_t* dev, uint8_t reg, uint8_t* buf,
                              uint8_t len);
static result_t write_register(icm20602_dev_t* dev, uint8_t reg, uint8_t* buf,
                               uint8_t len);

icm20602_dev_t icm20602_init(spi_inst_t* spi, const uint8_t cs_pin,
                             const icm20602_config_t* config) {
  if (!spi || !config) DIAG_REPORT_INVALID_ARG();

  result_t res = RESULT_OK;

  icm20602_dev_t dev = {
      .spi    = spi,
      .cs     = cs_pin,
      .config = *config,
  };

  // Initialize pins.
  gpio_init(cs_pin);
  gpio_set_dir(cs_pin, GPIO_OUT);
  gpio_put(cs_pin, CS_IDLE);

  uint8_t tmp = 0x00;

  // Reset.
  tmp = 0x80;
  res = write_register(&dev, REG_PWR_MGMT_1, &tmp, 1);
  if (res) exit(1);

  // TODO: Sleep 1000

  // Read something.
  res = read_register(&dev, REG_WHO_AM_I, &tmp, 1);
  if (res) exit(1);

  if (tmp != REG_WHO_AM_I_CONST) {
    diag_report_hardware(HARDWARE_STATUS_SPI_ERROR,
                         "ICM20602 WHO_AM_I mismatch");
    exit(1);
  }

  // Set clock to internal PLL.
  tmp = 0x01;
  res = write_register(&dev, REG_PWR_MGMT_1, &tmp, 1);
  if (res) exit(1);

  // Place accel and gryo on standby.
  tmp = 0x3F;
  res = write_register(&dev, REG_PWR_MGMT_2, &tmp, 1);
  if (res) exit(1);

  // Disable FIFO.
  tmp = 0x00;
  res = write_register(&dev, REG_USER_CTRL, &tmp, 1);
  if (res) exit(1);

  // Enable accelerometer.
  if (ICM20602_ACCEL_DLPF_BYPASS_1046_HZ == config->accel_dlpf) {
    tmp = (1 << 3);
    res = write_register(&dev, REG_ACCEL_CONFIG_2, &tmp, 1);
  } else {
    tmp = config->accel_dlpf;
    res = write_register(&dev, REG_ACCEL_CONFIG_2, &tmp, 1);
  }
  if (res) exit(1);

  tmp = (config->accel_range << 2);
  res = write_register(&dev, REG_ACCEL_CONFIG, &tmp, 1);
  if (res) exit(1);

  // Enable gyro.
  if (ICM20602_GYRO_DLPF_BYPASS_3281_HZ == config->gyro_dlpf) {
    // Bypass DLPF.
    tmp = 0x00;
    res = write_register(&dev, REG_CONFIG, &tmp, 1);
    if (res) exit(1);

    // Set range.
    tmp = (config->gyro_range << 3) | 0x02;
    res = write_register(&dev, REG_GYRO_CONFIG, &tmp, 1);
    if (res) exit(1);

  } else if (ICM20602_GYRO_DLPF_BYPASS_8173_HZ == config->gyro_dlpf) {
    // Bypass DLPF.
    tmp = 0x00;
    res = write_register(&dev, REG_CONFIG, &tmp, 1);
    if (res) exit(1);

    // Set range.
    tmp = (config->gyro_range << 3) | 0x01;
    res = write_register(&dev, REG_GYRO_CONFIG, &tmp, 1);
    if (res) exit(1);
  } else {
    // Configure DLPF.
    tmp = config->gyro_dlpf;
    res = write_register(&dev, REG_CONFIG, &tmp, 1);
    if (res) exit(1);

    // Set range.
    tmp = config->gyro_range << 3;
    res = write_register(&dev, REG_GYRO_CONFIG, &tmp, 1);
    if (res) exit(1);
  }

  // No FIFO.
  tmp = (config->accel_fifo) ? 0x08 : 0x00;
  tmp |= (config->gyro_fifo) ? 0x10 : 0x00;
  res = write_register(&dev, REG_FIFO_EN, &tmp, 1);
  if (res) exit(1);

  // Sample rate divider.
  tmp = (config->sample_rate_divider) ? (config->sample_rate_divider - 1) : 1;
  res = write_register(&dev, REG_SMPLRT_DIV, &tmp, 1);
  if (res) exit(1);

  tmp = 0;
  res = write_register(&dev, REG_PWR_MGMT_2, &tmp, 1);
  if (res) exit(1);

  return dev;
}

//
// Read three axes from the ICM20602.
//
static result_t read_axes_raw(icm20602_dev_t* dev, const int8_t addr,
                              int16_t* x, int16_t* y, int16_t* z) {
  if (!dev || !x || !y || !z) DIAG_REPORT_INVALID_ARG();

  uint8_t buf[6];

  result_t res = read_register(dev, addr, buf, 6);
  if (res) return RESULT_ERROR;

  *x = (buf[0] << 8) | buf[1];
  *y = (buf[2] << 8) | buf[3];
  *z = (buf[4] << 8) | buf[5];

  return RESULT_OK;
}

static const float GYRO_RANGES[4] = {
    [ICM20602_GYRO_RANGE_250_DPS]  = 250.f,
    [ICM20602_GYRO_RANGE_500_DPS]  = 500.f,
    [ICM20602_GYRO_RANGE_1000_DPS] = 1000.f,
    [ICM20602_GYRO_RANGE_2000_DPS] = 2000.f,
};

static const float ACCEL_RANGES[4] = {
    [ICM20602_ACCEL_RANGE_2_G]  = 2.f,
    [ICM20602_ACCEL_RANGE_4_G]  = 4.f,
    [ICM20602_ACCEL_RANGE_8_G]  = 8.f,
    [ICM20602_ACCEL_RANGE_16_G] = 16.f,
};

icm20602_data_t icm20602_read_gyro(icm20602_dev_t* dev) {
  if (!dev) DIAG_REPORT_INVALID_ARG();

  icm20602_data_t data = {
      .x = ICM20602_INVALID_VALUE,
      .y = ICM20602_INVALID_VALUE,
      .z = ICM20602_INVALID_VALUE,
  };

  const float gyro_range = GYRO_RANGES[dev->config.gyro_range];

  int16_t raw_x, raw_y, raw_z;
  result_t res = read_axes_raw(dev, REG_GYRO_XOUT_H, &raw_x, &raw_y, &raw_z);
  if (res) return data;

  // Convert to degrees per second, then to radians per second.
  data.x = ((float)raw_x / (float)INT16_MAX) * gyro_range * M_PI_180;
  data.y = ((float)raw_y / (float)INT16_MAX) * gyro_range * M_PI_180;
  data.z = ((float)raw_z / (float)INT16_MAX) * gyro_range * M_PI_180;

  return data;
}

icm20602_data_t icm20602_read_accel(icm20602_dev_t* dev) {
  if (!dev) DIAG_REPORT_INVALID_ARG();

  icm20602_data_t data = {
      .x = ICM20602_INVALID_VALUE,
      .y = ICM20602_INVALID_VALUE,
      .z = ICM20602_INVALID_VALUE,
  };

  const float accel_range = ACCEL_RANGES[dev->config.accel_range];

  int16_t raw_x, raw_y, raw_z;
  result_t res = read_axes_raw(dev, REG_ACCEL_XOUT_H, &raw_x, &raw_y, &raw_z);
  if (res) return data;

  // Convert to gravities, then remove the acceleration due to gravity to get
  // m/s^2.
  data.x = (((float)raw_x / (float)INT16_MAX) * accel_range) / GRAVITY_ACCEL;
  data.y = (((float)raw_y / (float)INT16_MAX) * accel_range) / GRAVITY_ACCEL;
  data.z = (((float)raw_z / (float)INT16_MAX) * accel_range) / GRAVITY_ACCEL;

  return data;
}

static result_t read_register(icm20602_dev_t* dev, uint8_t reg, uint8_t* buf,
                              uint8_t len) {
  if (!dev || !buf || !len) DIAG_REPORT_INVALID_ARG();

  const uint8_t cmd   = reg | 0x80; // MSB 1 for read.
  const uint8_t dummy = 0x00;

  gpio_put(dev->cs, CS_ACTIVE);

  if (1 != spi_write_blocking(dev->spi, &cmd, 1)) {
    diag_report_hardware(HARDWARE_STATUS_SPI_ERROR,
                         "ICM20602 read_register() failed: "
                         "could not write command byte");
    return RESULT_ERROR;
  }

  for (uint8_t i = 0; i < len; i++) {
    // Send dummy byte.
    if (1 != spi_write_blocking(dev->spi, &dummy, 1)) {
      diag_report_hardware(HARDWARE_STATUS_SPI_ERROR,
                           "ICM20602 read_register() failed: "
                           "could not write dummy byte");
      return RESULT_ERROR;
    }

    // Read byte.
    if (1 != spi_read_blocking(dev->spi, 0, &buf[i], 1)) {
      diag_report_hardware(HARDWARE_STATUS_SPI_ERROR,
                           "ICM20602 read_register() failed: "
                           "could not read byte");
      return RESULT_ERROR;
    }
  }

  gpio_put(dev->cs, CS_IDLE);

  return RESULT_OK;
}

static result_t write_register(icm20602_dev_t* dev, uint8_t reg, uint8_t* buf,
                               uint8_t len) {
  if (!dev || !buf || !len) DIAG_REPORT_INVALID_ARG();

  const uint8_t cmd = reg & 0x7F; // MSB 0 for write.

  gpio_put(dev->cs, CS_ACTIVE);

  if (1 != spi_write_blocking(dev->spi, &cmd, 1)) {
    diag_report_hardware(HARDWARE_STATUS_SPI_ERROR,
                         "ICM20602 write_register() failed: "
                         "could not write command byte");
    return RESULT_ERROR;
  }

  if (len != spi_write_blocking(dev->spi, buf, len)) {
    diag_report_hardware(HARDWARE_STATUS_SPI_ERROR,
                         "ICM20602 write_register() failed: "
                         "could not write data");
    return RESULT_ERROR;
  }

  gpio_put(dev->cs, CS_IDLE);

  return RESULT_OK;
}


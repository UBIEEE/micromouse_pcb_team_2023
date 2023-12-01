#include <micromouse/hardware/icm20602.h>

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

static result_t read_register(icm20602_dev_t* dev, uint8_t reg, uint8_t* buf, uint8_t len);
static result_t write_register(icm20602_dev_t* dev, uint8_t reg, uint8_t* buf, uint8_t len);

result_t icm20602_init(icm20602_dev_t* dev, spi_inst_t* spi, const uint8_t cs_pin,
                       const icm20602_config_t* cfg) {
  (void)cfg;
  // Initialize pins.
  gpio_init(cs_pin);
  gpio_set_dir(cs_pin, GPIO_OUT);
  gpio_put(cs_pin, CS_IDLE);

  dev->spi = spi;
  dev->cs  = cs_pin;

  uint8_t tmp = 0x00;
  uint8_t r   = 0;

  // Reset.
  tmp = 0x80;
  r   = write_register(dev, REG_PWR_MGMT_1, &tmp, 1);
  RETURN_IF_ERROR(r);

  // TODO: Sleep 1000

  // Read something.
  r = read_register(dev, REG_WHO_AM_I, &tmp, 1);
  if (tmp != REG_WHO_AM_I_CONST) {
    r = RESULT_ERROR;
  }
  RETURN_IF_ERROR(r);

  // Set clock to internal PLL.
  tmp = 0x01;
  r   = write_register(dev, REG_PWR_MGMT_1, &tmp, 1);
  RETURN_IF_ERROR(r);

  // Place accel and gryo on standby.
  tmp = 0x3F;
  r   = write_register(dev, REG_PWR_MGMT_2, &tmp, 1);
  RETURN_IF_ERROR(r);

  // Disable FIFO.
  tmp = 0x00;
  r   = write_register(dev, REG_USER_CTRL, &tmp, 1);
  RETURN_IF_ERROR(r);

  // Enable accelerometer.
  if (ICM20602_ACCEL_DLPF_BYPASS_1046_HZ == cfg->accel_dlpf) {
    tmp = (1 << 3);
    r   = write_register(dev, REG_ACCEL_CONFIG_2, &tmp, 1);
    RETURN_IF_ERROR(r);
  } else {
    tmp = cfg->accel_dlpf;
    r   = write_register(dev, REG_ACCEL_CONFIG_2, &tmp, 1);
    RETURN_IF_ERROR(r);
  }
  tmp = (cfg->accel_range << 2);
  r   = write_register(dev, REG_ACCEL_CONFIG, &tmp, 1);
  RETURN_IF_ERROR(r);

  // Enable gyro.
  if (ICM20602_GYRO_DLPF_BYPASS_3281_HZ == cfg->gyro_dlpf) {
    // Bypass DLPF.
    tmp = 0x00;
    r   = write_register(dev, REG_CONFIG, &tmp, 1);
    RETURN_IF_ERROR(r);

    // Set range.
    tmp = (cfg->gyro_range << 3) | 0x02;
    r   = write_register(dev, REG_GYRO_CONFIG, &tmp, 1);
    RETURN_IF_ERROR(r);
  } else if (ICM20602_GYRO_DLPF_BYPASS_8173_HZ == cfg->gyro_dlpf) {
    // Bypass DLPF.
    tmp = 0x00;
    r   = write_register(dev, REG_CONFIG, &tmp, 1);
    RETURN_IF_ERROR(r);

    // Set range.
    tmp = (cfg->gyro_range << 3) | 0x01;
    r   = write_register(dev, REG_GYRO_CONFIG, &tmp, 1);
    RETURN_IF_ERROR(r);
  } else {
    // Configure DLPF.
    tmp = cfg->gyro_dlpf;
    r   = write_register(dev, REG_CONFIG, &tmp, 1);
    RETURN_IF_ERROR(r);

    // Set range.
    tmp = cfg->gyro_range << 3;
    r   = write_register(dev, REG_GYRO_CONFIG, &tmp, 1);
    RETURN_IF_ERROR(r);
  }

  // No FIFO.
  tmp = (cfg->accel_fifo) ? 0x08 : 0x00;
  tmp |= (cfg->gyro_fifo) ? 0x10 : 0x00;
  r = write_register(dev, REG_FIFO_EN, &tmp, 1);
  RETURN_IF_ERROR(r);

  // Sample rate divider.
  tmp = (cfg->sample_rate_divider) ? (cfg->sample_rate_divider - 1) : 1;
  r   = write_register(dev, REG_SMPLRT_DIV, &tmp, 1);
  RETURN_IF_ERROR(r);

  tmp = 0;
  r   = write_register(dev, REG_PWR_MGMT_2, &tmp, 1);
  RETURN_IF_ERROR(r);

  return r;
}

//
// Read three axes from the ICM20602.
//
static result_t read_axes_raw(icm20602_dev_t* dev, const int8_t addr, int16_t* x, int16_t* y,
                              int16_t* z) {
  if (!dev || !x || !y || !z) {
    return RESULT_INVALID_ARG;
  }

  uint8_t buf[6];

  result_t result = read_register(dev, addr, buf, 6);
  if (result) {
    *x = *y = *z = ICM20602_INVALID_VALUE;
    return result;
  }

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

result_t icm20602_read_gyro(icm20602_dev_t* dev, float* x, float* y, float* z) {
  if (!dev || !x || !y || !z) {
    return RESULT_INVALID_ARG;
  }

  const float gyro_range = GYRO_RANGES[dev->config.gyro_range];

  int16_t raw_x, raw_y, raw_z;
  result_t result = read_axes_raw(dev, REG_GYRO_XOUT_H, &raw_x, &raw_y, &raw_z);
  RETURN_IF_ERROR(result);

  // Convert to degrees per second, then to radians per second.
  *x = ((float)raw_x / (float)INT16_MAX) * gyro_range * M_PI_180;
  *y = ((float)raw_y / (float)INT16_MAX) * gyro_range * M_PI_180;
  *z = ((float)raw_z / (float)INT16_MAX) * gyro_range * M_PI_180;

  return RESULT_OK;
}

result_t icm20602_read_accel(icm20602_dev_t* dev, float* x, float* y, float* z) {
  if (!dev || !x || !y || !z) {
    return RESULT_INVALID_ARG;
  }

  const float accel_range = ACCEL_RANGES[dev->config.accel_range];

  int16_t raw_x, raw_y, raw_z;
  result_t result = read_axes_raw(dev, REG_ACCEL_XOUT_H, &raw_x, &raw_y, &raw_z);
  RETURN_IF_ERROR(result);

  // Convert to gravities, then remove the acceleration due to gravity to get m/s^2.
  *x = (((float)raw_x / (float)INT16_MAX) * accel_range) / GRAVITY_ACCEL;
  *y = (((float)raw_y / (float)INT16_MAX) * accel_range) / GRAVITY_ACCEL;
  *z = (((float)raw_z / (float)INT16_MAX) * accel_range) / GRAVITY_ACCEL;

  return RESULT_OK;
}

static result_t read_register(icm20602_dev_t* dev, uint8_t reg, uint8_t* buf, uint8_t len) {
  if (!dev || !buf || !len) {
    return RESULT_INVALID_ARG;
  }

  const uint8_t cmd   = reg | 0x80; // MSB 1 for read.
  const uint8_t dummy = 0x00;

  gpio_put(dev->cs, CS_ACTIVE);

  if (1 != spi_write_blocking(dev->spi, &cmd, 1)) {
    return RESULT_SPI_COMM_ERROR;
  }

  for (uint8_t i = 0; i < len; i++) {
    // Send dummy byte.
    if (1 != spi_write_blocking(dev->spi, &dummy, 1)) {
      return RESULT_SPI_COMM_ERROR;
    }

    // Read byte.
    if (1 != spi_read_blocking(dev->spi, 0, &buf[i], 1)) {
      return RESULT_SPI_COMM_ERROR;
    }
  }

  gpio_put(dev->cs, CS_IDLE);

  return RESULT_OK;
}

static result_t write_register(icm20602_dev_t* dev, uint8_t reg, uint8_t* buf, uint8_t len) {
  if (!dev || !buf || !len) {
    return RESULT_INVALID_ARG;
  }

  const uint8_t cmd = reg & 0x7F; // MSB 0 for write.

  gpio_put(dev->cs, CS_ACTIVE);

  if (1 != spi_write_blocking(dev->spi, &cmd, 1)) {
    return RESULT_SPI_COMM_ERROR;
  }

  if (len != spi_write_blocking(dev->spi, buf, len)) {
    return RESULT_SPI_COMM_ERROR;
  }

  gpio_put(dev->cs, CS_IDLE);

  return RESULT_OK;
}


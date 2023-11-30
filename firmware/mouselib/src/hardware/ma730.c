#include <micromouse/hardware/ma730.h>

#define CS_IDLE   1
#define CS_ACTIVE 0

#define RAW_ANGLE_MAX       16384.f
#define RAW_ANGLE_2_RADIANS (M_2_PI / RAW_ANGLE_MAX)

result_t ma730_init(ma730_dev_t* dev, spi_inst_t* spi, const uint8_t cs_pin) {
  if (!dev || !spi) {
    return RESULT_INVALID_ARG;
  }

  // Initialize pins.
  gpio_init(cs_pin);
  gpio_set_dir(cs_pin, GPIO_OUT);
  gpio_put(cs_pin, CS_IDLE);

  // Construct handle.
  dev->spi = spi;
  dev->cs  = cs_pin;

  return RESULT_OK;
}

result_t ma730_read_angle(ma730_dev_t* dev, float* angle) {
  if (!dev || !angle) {
    *angle = MA730_ANGLE_INVALID;
    return RESULT_INVALID_ARG;
  }

  const uint16_t cmd = 0x0000;
  uint16_t value     = 0x0000;

  gpio_put(dev->cs, CS_ACTIVE);

  if (2 != spi_write16_read16_blocking(dev->spi, &cmd, &value, 1)) {
    *angle = MA730_ANGLE_INVALID;
    return RESULT_SPI_COMM_ERROR;
  }

  gpio_put(dev->cs, CS_IDLE);

  // 14-bit angle.
  // Read 16 bits (1 bit padding + 14 bit data + 1 bit padding)
  const uint16_t raw_angle = (value >> 1) & 0x3FFF;

  // Convert to radians.
  const float local_angle = (float)raw_angle * RAW_ANGLE_2_RADIANS;

  // Assign output.
  *angle = local_angle;

  return RESULT_OK;
}


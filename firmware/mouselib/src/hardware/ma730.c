#include <micromouse/hardware/ma730.h>

#include <micromouse/constants.h>
#include <micromouse/diagnostics.h>

#define CS_IDLE   1
#define CS_ACTIVE 0

#define RAW_ANGLE_MAX       16384.f
#define RAW_ANGLE_2_RADIANS (M_2_PI / RAW_ANGLE_MAX)

ma730_dev_t ma730_init(spi_inst_t* spi, const uint8_t cs_pin) {
  // Initialize pins.
  gpio_init(cs_pin);
  gpio_set_dir(cs_pin, GPIO_OUT);
  gpio_put(cs_pin, CS_IDLE);

  // Construct handle.
  ma730_dev_t dev = {
      .spi = spi,
      .cs  = cs_pin,
  };

  return dev;
}

float ma730_read_angle(ma730_dev_t* dev) {
  if (!dev) DIAG_REPORT_INVALID_ARG();

  const uint16_t cmd = 0x0000;
  uint16_t value     = 0x0000;

  gpio_put(dev->cs, CS_ACTIVE);

  if (2 != spi_write16_read16_blocking(dev->spi, &cmd, &value, 1)) {
    diag_report_hardware(HARDWARE_STATUS_SPI_ERROR,
                         "MA730 Failed to read angle");
    return MA730_ANGLE_INVALID;
  }

  gpio_put(dev->cs, CS_IDLE);

  // 14-bit angle.
  // Read 16 bits (1 bit padding + 14 bit data + 1 bit padding)
  const uint16_t raw_angle = (value >> 1) & 0x3FFF;

  // Convert to radians.
  const float local_angle = (float)raw_angle * RAW_ANGLE_2_RADIANS;

  // Assign output.
  return local_angle;
}


#include <micromouse/hardware/ma730.hpp>

#include <micromouse/macros.hpp>

using namespace hardware;

#define CS_IDLE   1
#define CS_ACTIVE 0

static constexpr uint16_t ANGLE_MAX = 16384;
static constexpr float ANGLE_TO_DEG = (360.f / ANGLE_MAX);

MA730::MA730(pico::spi_inst_t* spi, uint8_t cs_pin)
  : m_spi(spi),
    m_cs_pin(cs_pin) {

  if (!spi) INVALID_ARGS();

  // Initialize pins.
  pico::gpio_init(m_cs_pin);
  pico::gpio_set_dir(m_cs_pin, GPIO_OUT);
  pico::gpio_put(m_cs_pin, CS_IDLE);
}

float MA730::read_angle() const {
  const uint16_t cmd = 0x0000;
  uint16_t value     = 0x0000;

  pico::gpio_put(m_cs_pin, CS_ACTIVE);

  if (2 != pico::spi_write16_read16_blocking(m_spi, &cmd, &value, 1)) {
    FATAL_ERROR(TAG_SPI_ERROR, "MA730 Failed to read angle");
    return INVALID_ANGLE;
  }

  pico::gpio_put(m_cs_pin, CS_IDLE);

  // 14-bit angle.
  // Read 16 bits (1 bit padding + 14 bit data + 1 bit padding)
  const uint16_t raw_angle = (value >> 1) & 0x3FFF;

  // Convert to degrees.
  return static_cast<float>(raw_angle) * ANGLE_TO_DEG;
}


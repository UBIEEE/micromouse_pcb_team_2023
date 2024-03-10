#include <mouse_control/hardware/ma730.hpp>

#include <mouse_control/macros.hpp>

using namespace hardware;

#define CS_IDLE   1
#define CS_ACTIVE 0

static constexpr uint16_t ANGLE_MAX = 16384; // 2^14

MA730::MA730(pico::spi_inst_t* spi, uint8_t cs_pin)
  : m_spi(spi),
    m_cs_pin(cs_pin) {

  if (!spi) INVALID_ARGS();

  // Initialize pins.
  pico::gpio_init(m_cs_pin);
  pico::gpio_set_dir(m_cs_pin, GPIO_OUT);

  pico::gpio_put(m_cs_pin, CS_IDLE);

  m_last_angle = read_angle_raw() / float(ANGLE_MAX);
}

MA730::Data MA730::read(const float& dt) {
  const float angle = read_angle_raw() / float(ANGLE_MAX);

  float delta_angle = angle - m_last_angle;

  // Check for wrap around.
  if (delta_angle > .5f) {
    delta_angle -= 1.f;
  } else if (delta_angle < -.5f) {
    delta_angle += 1.f;
  }

  m_rotations += delta_angle;
  m_last_angle = angle;

  const float velocity = delta_angle / dt;

  return Data {m_rotations, velocity};
}

uint16_t MA730::read_angle_raw() const {
  const uint16_t cmd = 0x0000;
  uint16_t value     = 0x0000;

  pico::gpio_put(m_cs_pin, CS_ACTIVE);

  if (2 != pico::spi_write16_read16_blocking(m_spi, &cmd, &value, 1)) {
    FATAL_ERROR(TAG_SPI_ERROR, "MA730 Failed to read angle");
    return 0x0000;
  }

  pico::gpio_put(m_cs_pin, CS_IDLE);

  // 14-bit angle.
  // Read 16 bits (1 bit padding + 14 bit data + 1 bit padding)
  const uint16_t raw_angle = (value >> 1) & 0x3FFF;

  return raw_angle;
}

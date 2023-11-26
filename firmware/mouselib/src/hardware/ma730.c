#include <micromouse/hardware/ma730.h>

// Max baud rate is 25MHz.
#define BAUD_RATE (25 * 1000 * 1000)

#define CS_IDLE   1
#define CS_ACTIVE 0

#define MAX_ANGLE_VALUE 16384.f

static uint16_t read_register(ma730_handle_t* handle, uint8_t reg);

static void write_register(ma730_handle_t* handle, uint8_t reg,
                              uint8_t value);

void ma730_init_spi_instance(spi_inst_t* spi) {
  // Set baud rate.
  spi_set_baudrate(spi, BAUD_RATE);

  // Set format.
  spi_set_format(spi,
                 16,         // Numbers of bits per transfer
                 SPI_CPOL_1, // Polarity (CPOL)
                 SPI_CPHA_1, // Phase (CPHA)
                 SPI_MSB_FIRST);
}

ma730_handle_t ma730_init(spi_inst_t* spi, uint8_t cs_pin, uint8_t sclk_pin,
                          uint8_t mosi_pin, uint8_t miso_pin) {
  // Initialize pins.
  gpio_init(cs_pin);
  gpio_set_dir(cs_pin, GPIO_OUT);
  gpio_put(cs_pin, CS_IDLE);

  gpio_set_function(sclk_pin, GPIO_FUNC_SPI);
  gpio_set_function(mosi_pin, GPIO_FUNC_SPI);
  gpio_set_function(miso_pin, GPIO_FUNC_SPI);

  // Construct handle.
  ma730_handle_t handle = {
      .spi = spi,
      .cs = cs_pin,
  };

  return handle;
}

float ma730_read_angle(ma730_handle_t* handle) {
  uint8_t buf[2] = {0x00, 0x00};

  gpio_put(handle->cs, CS_ACTIVE);
  spi_write_blocking(handle->spi, buf, 2);
  spi_read_blocking(handle->spi, 0, buf, 2);
  gpio_put(handle->cs, CS_IDLE);

  uint16_t data = ((uint16_t)buf[0] << 8) | (uint16_t)buf[1];

  // 14-bit angle.
  // Read 16 bits (1 bit padding + 14 bit data + 1 bit padding)
  uint16_t raw_angle = (data >> 1) & 0x3FFF;

  // Convert to radians.
  float local_angle = raw_angle * (M_2_PI / MAX_ANGLE_VALUE);

  return local_angle;
}

static uint16_t read_register(ma730_handle_t* handle, uint8_t reg) {
  uint8_t buf[2];
  buf[0] = 0x40 | (reg & 0x1F); // Read address.
  buf[1] = 0x00;

  gpio_put(handle->cs, CS_ACTIVE);
  spi_write_blocking(handle->spi, buf, 2);
  spi_read_blocking(handle->spi, 0, buf, 2);
  gpio_put(handle->cs, CS_IDLE);

  return (buf[0] << 8) | buf[1];
}

static void write_register(ma730_handle_t* handle, uint8_t reg,
                              uint8_t value) {
  uint8_t buf[2];
  buf[0] = 0x80 | (reg & 0x1F); // Write address.
  buf[1] = value;               // Value.

  gpio_put(handle->cs, CS_ACTIVE);
  spi_write_blocking(handle->spi, buf, 2);
  gpio_put(handle->cs, CS_IDLE);
}

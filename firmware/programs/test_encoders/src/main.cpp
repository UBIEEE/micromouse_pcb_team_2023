#include <mouse_control/micromouse.hpp>
#include <mouse_control/constants.hpp>

int main() {
  // Don't init the MicroMouse singleton, just setup the SPI instance.
  pico::spi_inst_t* spi = MicroMouse::init_spi();

  hardware::MA730 enc_left(spi, PIN_L_ENC_CS);
  hardware::MA730 enc_right(spi, PIN_R_ENC_CS);

  while (true) {
    const float angle_left  = enc_left.read_angle();
    const float angle_right = enc_right.read_angle();

    printf("Left: %.2f, Right: %.2f\n", angle_left, angle_right);

    pico::sleep_ms(100);
  }

  return 0;
}


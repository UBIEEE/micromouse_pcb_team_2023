#include <mouse_control/constants.hpp>
#include <mouse_control/micromouse.hpp>

using namespace hardware;

static constexpr auto LOOP_MS = 200;
static constexpr auto LOOP_S  = 0.2f;

int main() {
  if (!pico::stdio_init_all()) return 1;

  // Don't init the MicroMouse singleton, just setup the SPI instance.
  pico::spi_inst_t* spi = MicroMouse::init_spi();

  MA730 enc_left(spi, Constants::IO::ENC_L_CS);
  MA730 enc_right(spi, Constants::IO::ENC_R_CS);

  while (true) {
    const MA730::Data data_left_rot  = enc_left.read(LOOP_S);
    const MA730::Data data_right_rot = enc_right.read(LOOP_S);

    printf("Left (rot):  %.2f, %.2f/s\n", data_left_rot.distance,
           data_left_rot.velocity);
    printf("Right (rot): %.2f, %.2f/s\n", data_right_rot.distance,
           data_right_rot.velocity);

    const MA730::Data data_left_m = data_left_rot.apply_coefficient(
        Constants::Mouse::WHEEL_CIRCUMFERENCE_M);
    const MA730::Data data_right_m = data_right_rot.apply_coefficient(
        Constants::Mouse::WHEEL_CIRCUMFERENCE_M);

    printf("Left (m):    %.2f, %.2f/s\n", data_left_m.distance,
           data_left_m.velocity);
    printf("Right (m):   %.2f, %.2f/s\n", data_right_m.distance,
           data_right_m.velocity);

    pico::sleep_ms(LOOP_MS);
  }

  return 0;
}

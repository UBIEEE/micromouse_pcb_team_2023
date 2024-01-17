#include <micromouse/std.h>

#include <micromouse/drive/drivetrain.h>
#include <micromouse/hardware/ma730.h>
#include <micromouse/io.h>

int main(void) {
  if (!stdio_init_all()) return 1;

  drivetrain_init_spi();

  ma730_dev_t left_encoder = ma730_init(spi0, PIN_L_ENC_CS);
  ma730_dev_t right_encoder = ma730_init(spi0, PIN_L_ENC_CS);

  float left_angle, right_angle;
  while (true) {
    left_angle = ma730_read_angle(&left_encoder);
    right_angle = ma730_read_angle(&right_encoder);

    (void)printf("left: %f, right: %f\n", left_angle, right_angle);

    sleep_ms(100);
  }

  return 0;
}


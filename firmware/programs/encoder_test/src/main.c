#include <micromouse/micromouse.h>

#include <micromouse/drivetrain.h>
#include <micromouse/hardware/ma730.h>
#include <micromouse/io.h>

result_t init_encoder(ma730_dev_t* encoder, uint cs_pin, const char* name) {
  result_t r = RESULT_OK;

  r = ma730_init(encoder, spi0, cs_pin);
  if (r) {
    (void)printf("ma730_init() failed for %s encoder: %#04X\n", name, r);
  }

  return r;
}

result_t read_angle(ma730_dev_t* encoder, float* angle, const char* name) {
  result_t r = RESULT_OK;

  r = ma730_read_angle(encoder, angle);
  if (r) {
    (void)printf("ma730_read_angle() failed for %s encoder: %#04X\n", name, r);
  }

  return r;
}

int main(void) {
  if (!stdio_init_all()) {
    (void)puts("stdio_init_all() failed");
    return -1;
  }

  result_t r = drivetrain_init_spi();
  if (r) {
    return (int)r;
  }


  ma730_dev_t left_encoder;
  ma730_dev_t right_encoder;

  r = init_encoder(&left_encoder, PIN_L_ENC_CS, "left");
  if (r) {
    return (int)r;
  }
  r = init_encoder(&right_encoder, PIN_R_ENC_CS, "right");
  if (r) {
    return (int)r;
  }

  float left_angle, right_angle;
  while (true) {
    r = read_angle(&left_encoder, &left_angle, "left");
    if (r) {
      break;
    }
    r = read_angle(&right_encoder, &right_angle, "right");
    if (r) {
      break;
    }

    (void)printf("left: %f, right: %f\n", left_angle, right_angle);

    sleep_ms(100);
  }

  return (int)r;
}


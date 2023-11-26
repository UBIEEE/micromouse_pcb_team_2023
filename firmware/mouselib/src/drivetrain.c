#include <micromouse/drivetrain.h>

#include <micromouse/io.h>

static float calc_angle_delta(float angle, float last_angle) {
  float delta = angle - last_angle;

  // Check for wrap around.
  if (delta > M_PI) {
    delta -= M_2_PI;
  } else if (delta < -M_PI) {
    delta += M_2_PI;
  }

  return delta;
}

drivetrain_handle_t drivetrain_init() {
  drivetrain_handle_t h;

  ma730_init_spi_instance(spi0);
  ma730_init_spi_instance(spi1);

  h.left_enc = ma730_init(spi0, PIN_L_ENC_CS, PIN_L_ENC_SCLK, PIN_L_ENC_MOSI,
                          PIN_L_ENC_MISO);

  h.right_enc = ma730_init(spi1, PIN_R_ENC_CS, PIN_R_ENC_SCLK, PIN_R_ENC_MOSI,
                           PIN_R_ENC_MISO);

  h.left_last_angle = h.left_angle = ma730_read_angle(&h.left_enc);
  h.right_last_angle = h.right_angle = ma730_read_angle(&h.right_enc);

  return h;
}

void drivetrain_process(drivetrain_handle_t* h) {
  // Update encoder angles.
  {
    // Read angles.
    float left_local_angle = ma730_read_angle(&h->left_enc);
    float right_local_angle = ma730_read_angle(&h->right_enc);

    // Calculate deltas.
    float left_delta = calc_angle_delta(left_local_angle, h->left_last_angle);
    float right_delta =
        calc_angle_delta(right_local_angle, h->right_last_angle);

    // Update angles.
    h->left_angle += left_delta;
    h->right_angle += right_delta;

    // Update last angles.
    h->left_last_angle = left_local_angle;
    h->right_last_angle = right_local_angle;
  }
}

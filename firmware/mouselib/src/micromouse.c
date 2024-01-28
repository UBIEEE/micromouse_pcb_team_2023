#include <micromouse/micromouse.h>

#include <micromouse/constants.h>
#include <micromouse/macros.h>
#include <micromouse/std.h>

#include <micromouse/hardware/drv8835.h>
#include <micromouse/hardware/icm20602.h>
#include <micromouse/hardware/ma730.h>

#define FIFO_FLAG_INIT 123

//
// ---
//

struct micromouse_hardware {
  ma730_dev_t enc_left;
  ma730_dev_t enc_right;

  drv8835_dev_t drive;

  icm20602_dev_t imu;
};
typedef struct micromouse_hardware micromouse_hardware_t;

struct micromouse_state {
  float enc_left_dist;
  float enc_right_dist;

  float enc_left_angle;
  float enc_right_angle;

  float enc_left_vel;
  float enc_right_vel;

  float imu_yaw_angle;
  float imu_yaw_vel;
};
typedef struct micromouse_state micromouse_state_t;

static micromouse_hardware_t s_hardware;

static micromouse_state_t s_state;
static mutex_t s_mutex; // Protects s_state.

//
// ---
//

static void micromouse_init(void);
static void micromouse_deinit(void);

static void primary_entry(void);   // Core 0
static void secondary_entry(void); // Core 1

typedef void (*periodic_func_t)(void);

// Creates a loop that calls periodic_func at a fixed rate.
static void loop(periodic_func_t periodic_func, uint32_t loop_period_ms);

// Ends a loop by sleeping until the end of the loop period.
static void loop_end(absolute_time_t begin_time, uint32_t loop_period_ms);

static void micromouse_fixed_update_primary(void);
static void micromouse_fixed_update_secondary(void);

//
// ---
//

void micromouse_main(void) {
  micromouse_init();

  multicore_launch_core1(secondary_entry);
  primary_entry();

  micromouse_deinit();
}

void micromouse_init(void) {
  // Set SPI baud rate.
  if (SPI0_BAUD_RATE != spi_set_baudrate(spi0, SPI0_BAUD_RATE)) {
    FATAL_ERROR(TAG_SPI_ERROR, "Failed to set SPI baud rate");
  }

  // Set SPI format.
  spi_set_format(spi0,
                 16,         // Numbers of bits per transfer
                 SPI_CPOL_1, // Polarity (CPOL)
                 SPI_CPHA_1, // Phase (CPHA)
                 SPI_MSB_FIRST);

  // Set which pins are used for SPI.
  gpio_set_function(PIN_IMU_R_ENC_SCLK, GPIO_FUNC_SPI);
  gpio_set_function(PIN_IMU_R_ENC_MOSI, GPIO_FUNC_SPI);
  gpio_set_function(PIN_IMU_R_ENC_MISO, GPIO_FUNC_SPI);
  gpio_set_function(PIN_L_ENC_SCLK, GPIO_FUNC_SPI);
  gpio_set_function(PIN_L_ENC_MOSI, GPIO_FUNC_SPI);
  gpio_set_function(PIN_L_ENC_MISO, GPIO_FUNC_SPI);

  // Encoders.
  s_hardware.enc_left  = ma730_init(spi0, PIN_L_ENC_CS);
  s_hardware.enc_right = ma730_init(spi0, PIN_R_ENC_CS);

  s_state.enc_left_dist   = 0.f;
  s_state.enc_right_dist  = 0.f;
  s_state.enc_left_angle  = ma730_read_angle(&s_hardware.enc_left);
  s_state.enc_right_angle = ma730_read_angle(&s_hardware.enc_right);
  s_state.enc_left_vel    = 0.f;
  s_state.enc_right_vel   = 0.f;

  // Drive.
  s_hardware.drive =
      drv8835_init(PIN_L_MOT_PH, PIN_L_MOT_EN, PIN_R_MOT_PH, PIN_R_MOT_EN);

  // IMU.
  const icm20602_config_t imu_config = {
      .accel_dlpf  = ICM20602_ACCEL_DLPF_10_2_HZ, // Just guessing here.
      .accel_range = ICM20602_ACCEL_RANGE_4_G,
      .accel_fifo  = false, // ??

      .gyro_dlpf  = ICM20602_GYRO_DLPF_10_HZ, // Guessing here too...
      .gyro_range = ICM20602_GYRO_RANGE_250_DPS,
      .gyro_fifo  = false, // ??

      .sample_rate_divider = 100, // ????
  };

  s_hardware.imu = icm20602_init(spi0, PIN_IMU_CS, &imu_config);

  // Mutex.
  mutex_init(&s_mutex);
}

void micromouse_deinit(void) {

  //

  drv8835_deinit(&s_hardware.drive);
}

static void primary_entry(void) {
  multicore_fifo_drain();

  // Wait for secondary core to initialize.
  const uint32_t g = multicore_fifo_pop_blocking();

  if (g != FIFO_FLAG_INIT) {
    panic("Core 1 isn't happy!");
  } else {
    // All's good on the secondary core.

    // Signal the secondary core that the primary core is good to go.
    multicore_fifo_push_blocking(FIFO_FLAG_INIT);
  }

  loop(micromouse_fixed_update_primary, LOOP_PERIOD_PRIMARY_MS);
}

static void secondary_entry(void) {
  multicore_fifo_drain();

  // Signal the primary core that the secondary core is good to go.
  multicore_fifo_push_blocking(FIFO_FLAG_INIT);

  // Wait for primary core to see that...
  const uint32_t g = multicore_fifo_pop_blocking();
  if (g != FIFO_FLAG_INIT) {
    panic("Core 0 isn't happy!");
  }

  loop(micromouse_fixed_update_secondary, LOOP_PERIOD_SECONDARY_MS);
}

static void loop(periodic_func_t periodic_func, uint32_t loop_period_ms) {
  absolute_time_t begin_time;
  while (true) {
    begin_time = get_absolute_time();

    periodic_func();

    // Sleep until the end of the loop period.
    loop_end(begin_time, loop_period_ms);
  }
}

static void loop_end(const absolute_time_t begin_time,
                     const uint32_t loop_period_ms) {

  const absolute_time_t end_time = get_absolute_time();

  // begin_time + loop_period_ms
  const absolute_time_t target_time = delayed_by_ms(begin_time, loop_period_ms);

  if (to_us_since_boot(end_time) >= to_us_since_boot(target_time)) {
    // Loop time exceeded...
    return;
  }

  sleep_until(target_time);
}

//
// --- Primary ---
//

static void micromouse_fixed_update_primary(void) {
  // TODO: Logic and control...

  // Aquire state.
  micromouse_state_t state;
  {
    mutex_enter_blocking(&s_mutex);
    state = s_state;
    mutex_exit(&s_mutex);
  }

  (void)state;
}

//
// --- Secondary ---
//

//
// Calculates the distance traveled since the last update based on the encoder's
// current angle its last angle.
// The angles should be represented in degrees, from 0-360.
//
static float calc_encoder_delta_distance(float angle, float last_angle);

static void micromouse_fixed_update_secondary(void) {
  // The other core never edits the state, so no need to lock.
  micromouse_state_t state = s_state;

  // IMU
  const float imu_yaw_vel =
      icm20602_read_gyro_axis(&s_hardware.imu, IMU_AXIS_YAW);

  state.imu_yaw_vel = imu_yaw_vel;
  state.imu_yaw_angle += imu_yaw_vel * LOOP_PERIOD_SECONDARY_MS;

  // Encoders
  const float enc_left_angle  = ma730_read_angle(&s_hardware.enc_left);
  const float enc_right_angle = ma730_read_angle(&s_hardware.enc_right);

  const float enc_left_delta_dist =
      calc_encoder_delta_distance(enc_left_angle, state.enc_left_angle);

  const float enc_right_delta_dist =
      calc_encoder_delta_distance(enc_right_angle, state.enc_right_angle);

  state.enc_left_angle  = enc_left_angle;
  state.enc_right_angle = enc_right_angle;

  state.enc_left_dist += enc_left_delta_dist;
  state.enc_right_dist += enc_right_delta_dist;

  state.enc_left_vel  = enc_left_delta_dist / LOOP_PERIOD_SECONDARY_MS;
  state.enc_right_vel = enc_right_delta_dist / LOOP_PERIOD_SECONDARY_MS;

  // Update state.
  {
    mutex_enter_blocking(&s_mutex);
    s_state = state;
    mutex_exit(&s_mutex);
  }
}

static float calc_encoder_delta_distance(const float angle,
                                         const float last_angle) {

  float delta_angle = angle - last_angle;

  // Check for wrap around.
  if (delta_angle > 180.f) {
    delta_angle -= 360.f;
  } else if (delta_angle < -180.f) {
    delta_angle += 360.f;
  }

  // Distance traveled.
  return (delta_angle / 360.f) * METERS_PER_ROTATION;
}


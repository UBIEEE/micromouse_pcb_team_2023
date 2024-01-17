#include <micromouse/drive/drivetrain.h>

#include <micromouse/diagnostics.h>
#include <micromouse/io.h>
#include <micromouse/macros.h>

// IMU max baud rate is 10MHz.
// Encoder max baud rate is 25MHz.
#define SPI_BAUD_RATE (10 * 1000 * 1000) // 10MHz

//
// --- Initialization ---
//

static void init_encoders(drivetrain_t* drivetrain);
static void init_imu(drivetrain_t* drivetrain);
static void init_motor_driver(drivetrain_t* drivetrain);

void drivetrain_init(drivetrain_t* drivetrain) {
  drivetrain_init_spi();

  init_encoders(drivetrain);
  init_imu(drivetrain);
  init_motor_driver(drivetrain);
}

void drivetrain_init_spi(void) {
  // Set SPI baud rate.
  if (SPI_BAUD_RATE != spi_set_baudrate(spi0, SPI_BAUD_RATE)) {
    diag_report_hardware(HARDWARE_STATUS_SPI_ERROR,
                         "Failed to set SPI baud rate");
    exit(1);
  }

  // Set format.
  spi_set_format(spi0,
                 16,         // Numbers of bits per transfer
                 SPI_CPOL_1, // Polarity (CPOL)
                 SPI_CPHA_1, // Phase (CPHA)
                 SPI_MSB_FIRST);

  // Set which pins are used.
  gpio_set_function(PIN_IMU_R_ENC_SCLK, GPIO_FUNC_SPI);
  gpio_set_function(PIN_IMU_R_ENC_MOSI, GPIO_FUNC_SPI);
  gpio_set_function(PIN_IMU_R_ENC_MISO, GPIO_FUNC_SPI);
  gpio_set_function(PIN_L_ENC_SCLK, GPIO_FUNC_SPI);
  gpio_set_function(PIN_L_ENC_MOSI, GPIO_FUNC_SPI);
  gpio_set_function(PIN_L_ENC_MISO, GPIO_FUNC_SPI);
}

static void init_encoders(drivetrain_t* drivetrain) {
  drivetrain->left_encoder  = ma730_init(spi0, PIN_L_ENC_CS);
  drivetrain->right_encoder = ma730_init(spi0, PIN_R_ENC_CS);

  // Read initial angles.
  drivetrain->left_angle  = ma730_read_angle(&drivetrain->left_encoder);
  drivetrain->right_angle = ma730_read_angle(&drivetrain->right_encoder);

  drivetrain->left_last_angle  = drivetrain->left_angle;
  drivetrain->right_last_angle = drivetrain->right_angle;
}

static void init_imu(drivetrain_t* drivetrain) {
  const icm20602_config_t imu_config = {
      .accel_dlpf  = ICM20602_ACCEL_DLPF_10_2_HZ, // Just guessing here.
      .accel_range = ICM20602_ACCEL_RANGE_4_G,
      .accel_fifo  = false, // ??

      .gyro_dlpf  = ICM20602_GYRO_DLPF_10_HZ, // Guessing here too...
      .gyro_range = ICM20602_GYRO_RANGE_250_DPS,
      .gyro_fifo  = false, // ??

      .sample_rate_divider = 100, // ????
  };

  drivetrain->imu = icm20602_init(spi0, PIN_IMU_CS, &imu_config);
}

static void init_motor_driver(drivetrain_t* drivetrain) {
  drivetrain->motor_driver =
      drv8835_init(PIN_L_MOT_PH, PIN_L_MOT_EN, PIN_R_MOT_PH, PIN_R_MOT_EN);
}

//
// --- Periodic stuff ---
//

//
// Calculates the delta between two angles (accounting for wrap around).
// The angles should be represented in radians.
//
static float calc_angle_delta(float angle, float last_angle);

static void update_encoder_angle(ma730_dev_t* dev, float* angle,
                                 float* last_angle);
static void update_imu(drivetrain_t* drivetrain);

void drivetrain_process(drivetrain_t* drivetrain) {
  // Update encoder angles.
  update_encoder_angle(&drivetrain->left_encoder, &drivetrain->left_angle,
                       &drivetrain->left_last_angle);

  update_encoder_angle(&drivetrain->right_encoder, &drivetrain->right_angle,
                       &drivetrain->right_last_angle);

  // Update IMU.
  update_imu(drivetrain);

  drv8835_set_output(&drivetrain->motor_driver, 0.f, 0.f);
}

static float calc_angle_delta(const float angle, const float last_angle) {
  float delta = angle - last_angle;

  // Check for wrap around.
  if (delta > M_PI) {
    delta -= M_2_PI;
  } else if (delta < -M_PI) {
    delta += M_2_PI;
  }

  return delta;
}

static void update_encoder_angle(ma730_dev_t* dev, float* angle,
                                 float* last_angle) {

  const float local_angle = ma730_read_angle(dev);

  // Calculate delta.
  const float delta = calc_angle_delta(local_angle, *last_angle);

  // Update angle.
  *angle += delta;
  *last_angle = local_angle;
}

static void update_imu(drivetrain_t* drivetrain) {
  // Gyro.
  drivetrain->imu_gyro_data = icm20602_read_gyro(&drivetrain->imu);

  // TODO: Integrate gyro readings to get angle.

  // Accelerometer.
  drivetrain->imu_accel_data = icm20602_read_accel(&drivetrain->imu);
}


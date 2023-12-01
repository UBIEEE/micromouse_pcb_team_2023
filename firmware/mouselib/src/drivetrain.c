#include <micromouse/drivetrain.h>

#include <micromouse/io.h>

// IMU max baud rate is 10MHz.
// Encoder max baud rate is 25MHz.
#define SPI_BAUD_RATE (10 * 1000 * 1000) // 10MHz

//
// --- Initialization ---
//

static result_t init_encoders(drivetrain_t* drivetrain);
static result_t init_imu(drivetrain_t* drivetrain);
static result_t init_motor_driver(drivetrain_t* drivetrain);

result_t drivetrain_init(drivetrain_t* drivetrain) {
  result_t r = RESULT_OK;

  r = drivetrain_init_spi();
  RETURN_IF_ERROR(r);

  r = init_encoders(drivetrain);
  RETURN_IF_ERROR(r);

  r = init_imu(drivetrain);
  RETURN_IF_ERROR(r);

  r = init_motor_driver(drivetrain);
  RETURN_IF_ERROR(r);

  return r;
}

result_t drivetrain_init_spi(void) {
  // Set SPI baud rate.
  if (SPI_BAUD_RATE != spi_set_baudrate(spi0, SPI_BAUD_RATE)) {
    return RESULT_ERROR;
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

  return RESULT_OK;
}

static result_t init_encoders(drivetrain_t* drivetrain) {
  result_t r = RESULT_OK;

  r = ma730_init(&drivetrain->left_encoder, spi0, PIN_L_ENC_CS);
  RETURN_IF_ERROR(r);
  r = ma730_init(&drivetrain->right_encoder, spi0, PIN_R_ENC_CS);
  RETURN_IF_ERROR(r);

  // Read initial angles.
  r = ma730_read_angle(&drivetrain->left_encoder, &drivetrain->left_last_angle);
  RETURN_IF_ERROR(r);
  r = ma730_read_angle(&drivetrain->right_encoder, &drivetrain->right_last_angle);
  RETURN_IF_ERROR(r);

  drivetrain->left_last_angle  = drivetrain->left_angle;
  drivetrain->right_last_angle = drivetrain->right_angle;

  return r;
}

static result_t init_imu(drivetrain_t* drivetrain) {
  result_t r = RESULT_OK;

  const icm20602_config_t imu_config = {
      .accel_dlpf  = ICM20602_ACCEL_DLPF_10_2_HZ, // Just guessing here.
      .accel_range = ICM20602_ACCEL_RANGE_4_G,
      .accel_fifo  = false, // ??

      .gyro_dlpf  = ICM20602_GYRO_DLPF_10_HZ, // Guessing here too...
      .gyro_range = ICM20602_GYRO_RANGE_250_DPS,
      .gyro_fifo  = false, // ??

      .sample_rate_divider = 100, // ????
  };

  r = icm20602_init(&drivetrain->imu, spi0, PIN_IMU_CS, &imu_config);
  RETURN_IF_ERROR(r);

  return r;
}

static result_t init_motor_driver(drivetrain_t* drivetrain) {
  result_t r = RESULT_OK;

  r = drv8835_init(&drivetrain->motor_driver, PIN_L_MOT_PH, PIN_L_MOT_EN, PIN_R_MOT_PH,
                   PIN_R_MOT_EN);

  RETURN_IF_ERROR(r);

  return r;
}

//
// --- Periodic stuff ---
//

//
// Calculates the delta between two angles (accounting for wrap around).
// The angles should be represented in radians.
//
static float calc_angle_delta(float angle, float last_angle);

static result_t update_encoder_angle(ma730_dev_t* dev, float* angle, float* last_angle);
static result_t update_imu(drivetrain_t* drivetrain);

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

static result_t update_encoder_angle(ma730_dev_t* dev, float* angle, float* last_angle) {
  float local_angle = 0.f;

  // Read local angle.
  result_t result = ma730_read_angle(dev, &local_angle);
  RETURN_IF_ERROR(result);

  // Calculate delta.
  const float delta = calc_angle_delta(local_angle, *last_angle);

  // Update angle.
  *angle += delta;
  *last_angle = local_angle;

  return RESULT_OK;
}

static result_t update_imu(drivetrain_t* drivetrain) {
  result_t result = RESULT_OK;
  float *x, *y, *z;

  //
  // Gyro.
  //
  x = &drivetrain->imu_gyro_x;
  y = &drivetrain->imu_gyro_y;
  z = &drivetrain->imu_gyro_z;

  result = icm20602_read_gyro(&drivetrain->imu, x, y, z);
  GOTO_IF_ERROR(result, error_leave);

  // TODO: Integrate gyro readings to get angle.

  //
  // Accelerometer.
  //
  x = &drivetrain->imu_accel_x;
  y = &drivetrain->imu_accel_y;
  z = &drivetrain->imu_accel_z;

  result = icm20602_read_accel(&drivetrain->imu, x, y, z);
  GOTO_IF_ERROR(result, error_leave);

  return result;

error_leave:
  *x = *y = *z = 0.f;
  return result;
}


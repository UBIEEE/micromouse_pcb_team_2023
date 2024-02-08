#include <mouse_control/micromouse.hpp>

#include <mouse_control/constants.hpp>
#include <mouse_control/macros.hpp>

#define FIFO_FLAG_INIT 123

MicroMouse::MicroMouse()
  : m_spi(init_spi()),
    m_enc_left(m_spi, PIN_L_ENC_CS),
    m_enc_right(m_spi, PIN_R_ENC_CS),
    m_motors(PIN_L_MOT_PH, PIN_L_MOT_EN, PIN_R_MOT_PH, PIN_R_MOT_EN),
    m_imu(m_spi, PIN_IMU_CS, hardware::ICM20602::Config {/*Default*/}) {}

pico::spi_inst_t* MicroMouse::init_spi() {
  // Set baud rate.
  if (SPI0_BAUD_RATE_HZ != pico::spi_set_baudrate(spi0, SPI0_BAUD_RATE_HZ)) {
    FATAL_ERROR(TAG_SPI_ERROR, "Failed to set SPI baud rate");
  }

  // Set format.
  pico::spi_set_format(spi0,
                       16,               // Numbers of bits per transfer
                       pico::SPI_CPOL_1, // Polarity (CPOL)
                       pico::SPI_CPHA_1, // Phase (CPHA)
                       pico::SPI_MSB_FIRST);

  // Initialize pins.
  gpio_set_function(PIN_IMU_R_ENC_SCLK, pico::GPIO_FUNC_SPI);
  gpio_set_function(PIN_IMU_R_ENC_MOSI, pico::GPIO_FUNC_SPI);
  gpio_set_function(PIN_IMU_R_ENC_MISO, pico::GPIO_FUNC_SPI);
  gpio_set_function(PIN_L_ENC_SCLK, pico::GPIO_FUNC_SPI);
  gpio_set_function(PIN_L_ENC_MOSI, pico::GPIO_FUNC_SPI);
  gpio_set_function(PIN_L_ENC_MISO, pico::GPIO_FUNC_SPI);

  return spi0;
}

void MicroMouse::run() {
  pico::multicore_launch_core1([]() { get().core1_entry(); });
  core0_entry();
}

void MicroMouse::core0_entry() {
  // Wait core 1 to initialize.
  const uint32_t g = pico::multicore_fifo_pop_blocking();

  if (g != FIFO_FLAG_INIT) {
    pico::panic("Core 1 isn't happy!");
  } else {
    // All's good on core 1.

    // Signal core 1 that core 0 is good to go.
    pico::multicore_fifo_push_blocking(FIFO_FLAG_INIT);
  }

  loop(&MicroMouse::primary_fixed_update,
       std::chrono::milliseconds(LOOP_PERIOD_PRIMARY_MS));
}

void MicroMouse::core1_entry() {
  // Signal core 0 that core 1 is good to go.
  pico::multicore_fifo_push_blocking(FIFO_FLAG_INIT);

  // Wait for core 0 to see that...
  const uint32_t g = pico::multicore_fifo_pop_blocking();
  if (g != FIFO_FLAG_INIT) {
    pico::panic("Core 0 isn't happy!");
  }

  loop(&MicroMouse::secondary_fixed_update,
       std::chrono::milliseconds(LOOP_PERIOD_SECONDARY_MS));
}

void MicroMouse::loop(LoopFunc func, std::chrono::milliseconds period) {
  while (true) {
    const pico::AbsoluteTime begin_time = pico::AbsoluteTime::now();

    (this->*func)(); // So weird

    const pico::AbsoluteTime target_time = begin_time + period;

    if (pico::AbsoluteTime::now() < target_time) {
      pico::sleep_until(target_time);
    }
  }
}

void MicroMouse::primary_fixed_update() {
  // Aquire sensor data.
  const SensorData sensor_data = ({
    std::lock_guard lock(m_sensor_data_mutex);
    m_sensor_data;
  });

  // TODO: Logic and control...

  (void)sensor_data;
}

void MicroMouse::secondary_fixed_update() {
  // The other core never edits the sensor data, so no need to lock it.
  SensorData sensor_data = m_sensor_data;

  // IMU.
  const float imu_yaw_rate_dps =
      m_imu.read_gyro_axis(hardware::ICM20602::Axis::Z);

  sensor_data.imu_yaw_rate_dps = imu_yaw_rate_dps;
  sensor_data.imu_yaw_angle_deg += imu_yaw_rate_dps * LOOP_PERIOD_SECONDARY_S;

  // Encoders.
  const float enc_left_angle_deg  = m_enc_left.read_angle();
  const float enc_right_angle_deg = m_enc_right.read_angle();

  const float enc_left_delta_dist_m = calc_encoder_delta_distance(
      enc_left_angle_deg, sensor_data.enc_left_angle_deg);

  const float enc_right_delta_dist_m = calc_encoder_delta_distance(
      enc_right_angle_deg, sensor_data.enc_right_angle_deg);

  sensor_data.enc_left_angle_deg  = enc_left_angle_deg;
  sensor_data.enc_right_angle_deg = enc_right_angle_deg;

  sensor_data.enc_left_dist_m += enc_left_delta_dist_m;
  sensor_data.enc_right_dist_m += enc_right_delta_dist_m;

  sensor_data.enc_left_vel_mps =
      enc_left_delta_dist_m / LOOP_PERIOD_SECONDARY_S;
  sensor_data.enc_right_vel_mps =
      enc_right_delta_dist_m / LOOP_PERIOD_SECONDARY_S;

  // TODO: Other sensors...

  // Update sensor data.
  std::lock_guard lock(m_sensor_data_mutex);
  m_sensor_data = sensor_data;
}

float MicroMouse::calc_encoder_delta_distance(const float angle,
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


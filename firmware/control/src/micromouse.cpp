#include <mouse_control/micromouse.hpp>

#include <mouse_control/constants.hpp>
#include <mouse_control/macros.hpp>

#define FIFO_FLAG_INIT 123

MicroMouse::MicroMouse()
  : m_spi(init_spi()),
#if ODOMETRY_ENCODERS
    m_enc_left(m_spi, Constants::IO::ENC_L_CS),
    m_enc_right(m_spi, Constants::IO::ENC_R_CS),
#endif
    m_motors(Constants::IO::MOT_L_PH, Constants::IO::MOT_L_EN,
             Constants::IO::MOT_R_PH, Constants::IO::MOT_R_EN),
    m_imu(m_spi, Constants::IO::IMU_CS,
          hardware::ICM20602::Config {/*Default*/}) {
}

pico::spi_inst_t* MicroMouse::init_spi() {
  // Set baud rate.
  if (Constants::Hardware::SPI0_BAUD_RATE_HZ !=
      pico::spi_set_baudrate(spi0, Constants::Hardware::SPI0_BAUD_RATE_HZ)) {
    FATAL_ERROR(TAG_SPI_ERROR, "Failed to set SPI baud rate");
  }

  // Set format.
  pico::spi_set_format(spi0,
                       16,               // Numbers of bits per transfer
                       pico::SPI_CPOL_1, // Polarity (CPOL)
                       pico::SPI_CPHA_1, // Phase (CPHA)
                       pico::SPI_MSB_FIRST);

  // Initialize pins.
  gpio_set_function(Constants::IO::IMU_ENC_R_SCLK, pico::GPIO_FUNC_SPI);
  gpio_set_function(Constants::IO::IMU_ENC_R_MOSI, pico::GPIO_FUNC_SPI);
  gpio_set_function(Constants::IO::IMU_ENC_R_MISO, pico::GPIO_FUNC_SPI);
  gpio_set_function(Constants::IO::ENC_L_SCLK, pico::GPIO_FUNC_SPI);
  gpio_set_function(Constants::IO::ENC_L_MOSI, pico::GPIO_FUNC_SPI);
  gpio_set_function(Constants::IO::ENC_L_MISO, pico::GPIO_FUNC_SPI);

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
       std::chrono::milliseconds(Constants::Runtime::LOOP_PERIOD_PRIMARY_MS));
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
       std::chrono::milliseconds(Constants::Runtime::LOOP_PERIOD_SECONDARY_MS));
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
  // Acquire sensor data.
  const SensorData sensor_data = ({
    std::lock_guard lock(m_sensor_data_mutex);
    m_sensor_data;
  });

  (void)sensor_data;

  // TODO: Logic and control...
}

void MicroMouse::secondary_fixed_update() {
  // The other core never edits the sensor data, so no need to lock it.
  SensorData sensor_data = m_sensor_data;

  using namespace hardware;

  // IMU.
  const float imu_yaw_rate_dps = m_imu.read_gyro_axis(ICM20602::Axis::Z);

  sensor_data.imu_yaw_rate_dps = imu_yaw_rate_dps;
  sensor_data.imu_yaw_angle_deg +=
      imu_yaw_rate_dps * Constants::Runtime::LOOP_PERIOD_SECONDARY_S;

  // Encoders.
#if ODOMETRY_ENCODERS
  const MA730::Data enc_left_data =
      m_enc_left.read(Constants::Runtime::LOOP_PERIOD_SECONDARY_S)
          .apply_coefficient(Constants::Mouse::WHEEL_CIRCUMFERENCE_M);
  const MA730::Data enc_right_data =
      m_enc_right.read(Constants::Runtime::LOOP_PERIOD_SECONDARY_S)
          .apply_coefficient(Constants::Mouse::WHEEL_CIRCUMFERENCE_M);

  sensor_data.enc_left_dist_m  = enc_left_data.distance;
  sensor_data.enc_right_dist_m = enc_right_data.distance;

  sensor_data.enc_left_vel_mps  = enc_left_data.velocity;
  sensor_data.enc_right_vel_mps = enc_right_data.velocity;
#endif

  // TODO: Other sensors...

  // Update sensor data.
  std::lock_guard lock(m_sensor_data_mutex);
  m_sensor_data = sensor_data;
}

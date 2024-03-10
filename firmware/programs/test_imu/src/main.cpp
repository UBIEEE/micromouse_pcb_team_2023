#include <mouse_control/constants.hpp>
#include <mouse_control/micromouse.hpp>

using namespace hardware;

static constexpr auto LOOP_MS = 200;

int main() {
  if (!pico::stdio_init_all()) return 1;

  // Don't init the MicroMouse singleton, just setup the SPI instance.
  pico::spi_inst_t* spi = MicroMouse::init_spi();

  const hardware::ICM20602::Config imu_config {};
  ICM20602 imu(spi, Constants::IO::IMU_CS, imu_config);

  while (true) {
    ICM20602::Data accel = imu.read_accel();
    ICM20602::Data gyro  = imu.read_gyro();

    printf("Accel: x: %f, y: %f, z: %f\n", accel.x, accel.y, accel.z);
    printf("Gyro:  x: %f, y: %f, z: %f\n", gyro.x, gyro.y, gyro.z);

    pico::sleep_ms(LOOP_MS);
  }

  return 0;
}

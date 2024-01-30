#include <micromouse/micromouse.hpp>
#include <micromouse/constants.hpp>

int main() {
  // Don't init the MicroMouse singleton, just setup the SPI instance.
  pico::spi_inst_t* spi = MicroMouse::init_spi();

  const hardware::ICM20602::Config imu_config {};
  hardware::ICM20602 imu(spi, PIN_IMU_CS, imu_config);

  while (true) {
    hardware::ICM20602::Data accel = imu.read_accel();
    hardware::ICM20602::Data gyro  = imu.read_gyro();

    printf("Accel: x: %f, y: %f, z: %f\n", accel.x, accel.y, accel.z);
    printf("Gyro:  x: %f, y: %f, z: %f\n", gyro.x, gyro.y, gyro.z);

    pico::sleep_ms(100);
  }

  return 0;
}


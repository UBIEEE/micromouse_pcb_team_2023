#include <micromouse/micromouse.h>

#include <micromouse/drivetrain.h>
#include <micromouse/hardware/icm20602.h>
#include <micromouse/io.h>

int main(void) {
  if (!stdio_init_all()) {
    (void)puts("stdio_init_all() failed");
    return -1;
  }

  drivetrain_init_spi();

  icm20602_dev_t imu;

  const icm20602_config_t imu_config = {
      .accel_dlpf  = ICM20602_ACCEL_DLPF_10_2_HZ, // Just guessing here.
      .accel_range = ICM20602_ACCEL_RANGE_4_G,
      .accel_fifo  = false, // ??

      .gyro_dlpf  = ICM20602_GYRO_DLPF_10_HZ, // Guessing here too...
      .gyro_range = ICM20602_GYRO_RANGE_250_DPS,
      .gyro_fifo  = false, // ??

      .sample_rate_divider = 100, // ????
  };

  result_t r = icm20602_init(&imu, spi0, PIN_IMU_CS, &imu_config);
  if (r) {
    (void)printf("icm20602_init() failed: %#04X\n", r);
    return (int)r;
  }

  float x, y, z;
  while (true) {
    r = icm20602_read_accel(&imu, &x, &y, &z);
    if (r) {
      (void)printf("icm20602_read_accel() failed: %#04X\n", r);
      break;
    }

    (void)printf("x: %f, y: %f, z: %f\n", x, y, z);
    sleep_ms(100);
  }

  return (int)r;
}
 

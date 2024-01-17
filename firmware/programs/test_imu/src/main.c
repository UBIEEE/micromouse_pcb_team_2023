#include <micromouse/std.h>

#include <micromouse/drive/drivetrain.h>
#include <micromouse/hardware/icm20602.h>
#include <micromouse/io.h>

int main(void) {
  if (!stdio_init_all()) return 1;

  drivetrain_init_spi();

  const icm20602_config_t imu_config = {
      .accel_dlpf  = ICM20602_ACCEL_DLPF_10_2_HZ, // Just guessing here.
      .accel_range = ICM20602_ACCEL_RANGE_4_G,
      .accel_fifo  = false, // ??

      .gyro_dlpf  = ICM20602_GYRO_DLPF_10_HZ, // Guessing here too...
      .gyro_range = ICM20602_GYRO_RANGE_250_DPS,
      .gyro_fifo  = false, // ??

      .sample_rate_divider = 100, // ????
  };

  icm20602_dev_t imu = icm20602_init(spi0, PIN_IMU_CS, &imu_config);

  icm20602_data_t gyro_data;
  icm20602_data_t accel_data;
  while (true) {
    gyro_data  = icm20602_read_accel(&imu);
    accel_data = icm20602_read_gyro(&imu);

    (void)printf("GYRO => x: %f, y: %f, z: %f\n", gyro_data.x, gyro_data.y,
                 gyro_data.z);
    (void)printf("ACCEL => x: %f, y: %f, z: %f\n", accel_data.x, accel_data.y,
                 accel_data.z);

    sleep_ms(100);
  }

  return 0;
}


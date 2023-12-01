#ifndef __MICROMOUSE_ICM20602_H__
#define __MICROMOUSE_ICM20602_H__

#include <micromouse/micromouse.h>

//
// Datasheet:
// https://invensense.tdk.com/wp-content/uploads/2016/10/DS-000176-ICM-20602-v1.0.pdf
//

//
// A_DLPF_CFG in the ACCEL_CONFIG2 register, except for BYPASS cases.
//
enum icm20602_accel_dlpf {
  ICM20602_ACCEL_DLPF_218_1_HZ = 0,
  ICM20602_ACCEL_DLPF_99_HZ    = 2,
  ICM20602_ACCEL_DLPF_44_8_HZ  = 3,
  ICM20602_ACCEL_DLPF_21_2_HZ  = 4,
  ICM20602_ACCEL_DLPF_10_2_HZ  = 5,
  ICM20602_ACCEL_DLPF_5_1_HZ   = 6,
  ICM20602_ACCEL_DLPF_420_HZ   = 7,
  ICM20602_ACCEL_DLPF_BYPASS_1046_HZ, // No filter
};
typedef enum icm20602_accel_dlpf icm20602_accel_dlpf_t;

//
// ACCEL_FS_SEL in the ACCEL_CONFIG register.
//
enum icm20602_accel_range {
  ICM20602_ACCEL_RANGE_2_G  = 0,
  ICM20602_ACCEL_RANGE_4_G  = 1,
  ICM20602_ACCEL_RANGE_8_G  = 2,
  ICM20602_ACCEL_RANGE_16_G = 3,
};
typedef enum icm20602_accel_range icm20602_accel_range_t;

//
// DLPF_CFG in the CONFIG register, except for BYPASS cases.
//
enum icm20602_gyro_dlpf {
  ICM20602_GYRO_DLPF_250_HZ  = 0,
  ICM20602_GYRO_DLPF_176_HZ  = 1,
  ICM20602_GYRO_DLPF_92_HZ   = 2,
  ICM20602_GYRO_DLPF_41_HZ   = 3,
  ICM20602_GYRO_DLPF_20_HZ   = 4,
  ICM20602_GYRO_DLPF_10_HZ   = 5,
  ICM20602_GYRO_DLPF_5_HZ    = 6,
  ICM20602_GYRO_DLPF_3281_HZ = 7,
  ICM20602_GYRO_DLPF_BYPASS_3281_HZ, // No filter
  ICM20602_GYRO_DLPF_BYPASS_8173_HZ, // No filter
};
typedef enum icm20602_gyro_dlpf icm20602_gyro_dlpf_t;

//
// FS_SEL in the GYRO_CONFIG register.
//
// Note: DPS refers to degrees per second.
//
enum icm20602_gyro_range {
  ICM20602_GYRO_RANGE_250_DPS  = 0,
  ICM20602_GYRO_RANGE_500_DPS  = 1,
  ICM20602_GYRO_RANGE_1000_DPS = 2,
  ICM20602_GYRO_RANGE_2000_DPS = 3,
};
typedef enum icm20602_gyro_range icm20602_gyro_range_t;

//
// Configuration for an ICM20602 IMU.
//
struct icm20602_config {
  icm20602_accel_dlpf_t accel_dlpf;
  icm20602_accel_range_t accel_range;
  bool accel_fifo;

  icm20602_gyro_dlpf_t gyro_dlpf;
  icm20602_gyro_range_t gyro_range;
  bool gyro_fifo;

  uint8_t sample_rate_divider;
};
typedef struct icm20602_config icm20602_config_t;

//
// Represents an ICM20602 IMU device over SPI.
//
struct icm20602_dev {
  spi_inst_t* spi;
  uint8_t cs;
  icm20602_config_t config;
};
typedef struct icm20602_dev icm20602_dev_t;

#define ICM20602_INVALID_VALUE (-1.f)

//
// Initializes an ICM20602 IMU.
//
// Important: The SPI instance must be initialized prior to calling this.
//
result_t icm20602_init(icm20602_dev_t* dev, spi_inst_t* spi, uint8_t cs_pin,
                       const icm20602_config_t* config);

//
// Reads the gyro values from the IMU.
//
// Gyro value units are radians per second.
//
result_t icm20602_read_gyro(icm20602_dev_t* dev, float* x, float* y, float* z);

//
// Reads the accelerometer values from the IMU.
//
// Accelerometer value units are meters per second squared.
//
result_t icm20602_read_accel(icm20602_dev_t* dev, float* x, float* y, float* z);

#endif // __MICROMOUSE_ICM20602_H__


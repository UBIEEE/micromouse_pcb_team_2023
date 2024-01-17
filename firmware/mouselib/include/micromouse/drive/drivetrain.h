#ifndef __MICROMOUSE_DRIVETRAIN_H__
#define __MICROMOUSE_DRIVETRAIN_H__

#include <micromouse/std.h>

#include <micromouse/hardware/drv8835.h>
#include <micromouse/hardware/icm20602.h>
#include <micromouse/hardware/ma730.h>

struct drivetrain {
  ma730_dev_t left_encoder;
  ma730_dev_t right_encoder;

  float left_last_angle;
  float right_last_angle;

  float left_angle;
  float right_angle;

  icm20602_dev_t imu;

  // Rotational velocities (rad/s).
  icm20602_data_t imu_gyro_data;

  // Linear accelerations (m/s^2).
  icm20602_data_t imu_accel_data;

  drv8835_dev_t motor_driver;
};
typedef struct drivetrain drivetrain_t;

//
// Initializes the drivetrain hardware.
// The program will exit if the drivetrain could not be initialized.
//
void drivetrain_init(drivetrain_t* drivetrain);

//
// Initializes the spi0 instance used by the encoders and IMU.
// The program will exit if spi0 could not be initialized.
//
void drivetrain_init_spi(void);

//
// Handles the drivetrain's periodic tasks.
//
void drivetrain_process(drivetrain_t* drivetrain);

#endif // __MICROMOUSE_DRIVETRAIN_H__


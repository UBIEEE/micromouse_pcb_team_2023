#ifndef __MICROMOUSE_DRIVETRAIN_H__
#define __MICROMOUSE_DRIVETRAIN_H__

#include <micromouse/micromouse.h>
#include <micromouse/hardware/ma730.h>
#include <micromouse/hardware/icm20602.h>

struct drivetrain {
  ma730_dev_t left_encoder;
  ma730_dev_t right_encoder;

  float left_last_angle;
  float right_last_angle;

  float left_angle;
  float right_angle;

  icm20602_dev_t imu;

  // Rotational velocities (rad/s)
  float imu_gyro_x;
  float imu_gyro_y;
  float imu_gyro_z;

  // Linear accelerations (m/s^2)
  float imu_accel_x;
  float imu_accel_y;
  float imu_accel_z;
};
typedef struct drivetrain drivetrain_t;

//
// Initializes the drivetrain hardware.
//
result_t drivetrain_init(drivetrain_t* drivetrain);

//
// Handles the drivetrain's periodic tasks.
//
void drivetrain_process(drivetrain_t* drivetrain);

#endif // __MICROMOUSE_DRIVETRAIN_H__

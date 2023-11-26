#ifndef __MICROMOUSE_DRIVETRAIN_H__
#define __MICROMOUSE_DRIVETRAIN_H__

#include <micromouse/micromouse.h>
#include <micromouse/hardware/ma730.h>

typedef struct drivetrain_handle_t {
  ma730_handle_t left_enc;
  ma730_handle_t right_enc;

  float left_last_angle;
  float right_last_angle;

  float left_angle;
  float right_angle;
} drivetrain_handle_t;

//
// Initializes the drivetrain hardware.
//
drivetrain_handle_t drivetrain_init();

//
// Handles the drivetrain's periodic tasks.
//
void drivetrain_process(drivetrain_handle_t* handle);

#endif // __MICROMOUSE_DRIVETRAIN_H__

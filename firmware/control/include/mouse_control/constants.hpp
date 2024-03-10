#pragma once

#include <mouse_control/std.hpp>

struct Constants {
#define CONSTANT static constexpr

  struct Hardware {
    CONSTANT int CLOCK_FREQ_HZ = 125'000'000;

    // IMU max baud rate is 10MHz.
    // Encoder max baud rate is 25MHz.
    CONSTANT int SPI0_BAUD_RATE_HZ = 10'000'000; // 10MHz
  };

  struct Mouse {
    CONSTANT float WHEEL_DIAMETER_M = 0.020f; // 20mm
    CONSTANT float WHEEL_CIRCUMFERENCE_M =
        WHEEL_DIAMETER_M * std::numbers::pi_v<float>;
  };

  struct IO {
    CONSTANT int BTN_1 = 0;
    CONSTANT int BTN_2 = 1;

    // SPI 0
    CONSTANT int IMU_ENC_R_SCLK = 2; // SPI0 SCK
    CONSTANT int IMU_ENC_R_MOSI = 3; // SPI0 TX
    CONSTANT int IMU_ENC_R_MISO = 4; // SPI0 RX
    CONSTANT int IMU_CS         = 5; // SPI0 CSn

    CONSTANT int MOT_L_PH = 6; // PWM3 A
    CONSTANT int MOT_L_EN = 7;

    CONSTANT int MOT_R_PH = 8; // PWM4 A
    CONSTANT int MOT_R_EN = 9;

    // 10 - Unused

    CONSTANT int ENC_R_CS = 11;

    // 12 - Unused
    // 13 - Unused
    // 14 - Unused
    // 15 - Unused

    // SPI 0
    CONSTANT int ENC_L_SCLK = 18; // SPI0 SCK
    CONSTANT int ENC_L_MOSI = 19; // SPI0 TX
    CONSTANT int ENC_L_MISO = 16; // SPI0 RX
    CONSTANT int ENC_L_CS   = 17; // SPI0 CSn

    // 20 - Unused

    CONSTANT int LED_1 = 21;
    CONSTANT int LED_2 = 22;
    CONSTANT int LED_3 = 23;
    CONSTANT int LED_4 = 24;
    CONSTANT int LED_5 = 25;

    CONSTANT int ADC_0 = 26;
    CONSTANT int ADC_1 = 27;
    CONSTANT int ADC_2 = 28;
    CONSTANT int ADC_3 = 29;
  };

  struct Physics {
    // Acceleration due to gravity (m/s^2).
    CONSTANT float GRAVITY_ACCEL_MPS2 = 9.80665f;
  };

  struct Runtime {
    // Primary loop, used for logic and control.
    CONSTANT int LOOP_PERIOD_PRIMARY_MS  = 20;
    CONSTANT float LOOP_PERIOD_PRIMARY_S = LOOP_PERIOD_PRIMARY_MS / 1000.f;

    //
    // Secondary loop period, used for sampling sensors.
    //
    // Max imu velocity is 250 deg/s, so the sampling frequency should be double
    // that. (1/(250*2))*1000 = 2ms
    //
    // TODO: Will this work well for the encoders? We don't want to miss a
    // rotation...
    //
    CONSTANT int LOOP_PERIOD_SECONDARY_MS  = 2;
    CONSTANT float LOOP_PERIOD_SECONDARY_S = LOOP_PERIOD_SECONDARY_MS / 1000.f;
  };

#undef CONSTANT
};

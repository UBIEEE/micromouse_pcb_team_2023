#pragma once

//
// --- Mouse hardware constants ---
//

#define CLOCK_FREQ_HZ 125'000'000

// IMU max baud rate is 10MHz.
// Encoder max baud rate is 25MHz.
#define SPI0_BAUD_RATE_HZ 10'000'000 // 10MHz

//
// Meters traveled per encoder rotation.
// TODO: Measure this.
//
#define METERS_PER_ROTATION 0.1f

//
// --- Mouse IO map ---
//

#define PIN_BTN_1 0
#define PIN_BTN_2 1

// SPI 0
#define PIN_IMU_R_ENC_SCLK 2
#define PIN_IMU_R_ENC_MOSI 3
#define PIN_IMU_R_ENC_MISO 4
#define PIN_IMU_CS         5

#define PIN_L_MOT_PH 6
#define PIN_L_MOT_EN 7

#define PIN_R_MOT_PH 8
#define PIN_R_MOT_EN 9

// 10 - Unused

#define PIN_R_ENC_CS 11

// 12 - Unused
// 13 - Unused
// 14 - Unused
// 15 - Unused

// SPI 0
#define PIN_L_ENC_SCLK 18
#define PIN_L_ENC_MOSI 19
#define PIN_L_ENC_MISO 16
#define PIN_L_ENC_CS   17

// 20 - Unused

#define PIN_LED_1 21
#define PIN_LED_2 22
#define PIN_LED_3 23
#define PIN_LED_4 24
#define PIN_LED_5 25

#define PIN_ADC_0 26
#define PIN_ADC_1 27
#define PIN_ADC_2 28
#define PIN_ADC_3 29

//
// --- Extra math constants ---
//

#define M_PI_180 0.01745329252f // π/180 (Degrees -> radians)
#define M_180_PI 57.2957795131f // 180/π (Radians -> degrees)

#define DEG_TO_RAD M_PI_180
#define RAD_TO_DEG M_180_PI

//
// --- Physical constants ---
//

// Acceleration due to gravity (m/s^2).
#define GRAVITY_ACCEL 9.80665f

//
// --- Runtime constants ---
//

// Primary loop, used for logic and control.
#define LOOP_PERIOD_PRIMARY_MS 20
#define LOOP_PERIOD_PRIMARY_S (LOOP_PERIOD_PRIMARY_MS / 1000.f)

//
// Secondary loop period, used for sampling sensors.
//
// Max imu velocity is 250 deg/s, so the sampling frequency should be double
// that. (1/(250*2))*1000 = 2ms
//
// TODO: Will this work well for the encoders? We don't want to miss a
// rotation...
//
#define LOOP_PERIOD_SECONDARY_MS 2
#define LOOP_PERIOD_SECONDARY_S (LOOP_PERIOD_SECONDARY_MS / 1000.f)

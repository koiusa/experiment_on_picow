/**
 * @file struct_ds4_on_pico_w.hpp
 * @brief
 * @version 0.1
 * @date 2024-12-25
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef STRUCT_DS4_FOR_PICO_W_HPP
#define STRUCT_DS4_FOR_PICO_W_HPP
#include <stdint.h>

struct DualShock4_state {
    // Directional buttons
    uint16_t hat;
    // Share button
    bool share;
    // Options button
    bool options;
    // PS button
    bool ps;

    // Action Buttons
    bool triangle;
    bool square;
    bool circle;
    bool cross;

    // Touch pad/Touch pad button
    //  To use the touch pad button, simply press the touch pad.
    bool touch;
    uint16_t touch_x;
    uint16_t touch_y;

    // Top left button
    bool l1;
    bool l2;
    uint16_t l2_value;
    // Top Right button
    bool r1;
    bool r2;
    uint16_t r2_value;

    // Left stick/L3 button
    //    Press on the stick to use it as the R3 button.
    bool l3;
    uint16_t l3_x;
    uint16_t l3_y;

    // Right stick / R3 button
    //    Press on the stick to use it as the L3 button.
    bool r3;
    uint16_t r3_x;
    uint16_t r3_y;

    // Gyro sensor
    uint16_t gyro_x;
    uint16_t gyro_y;
    uint16_t gyro_z;

    // Acceleration sensor
    uint16_t accel_x;
    uint16_t accel_y;
    uint16_t accel_z;

    // Battery level
    uint16_t battery;

    // Temperature
    uint16_t temperature;

    // Timestamp
    uint16_t timestamp;

    // Status
    bool connected;
};

#endif // STRUCT_DS4_HPP

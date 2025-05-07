#include "calibrater.h"

Calibrater::Calibrater(/* args */)
{
    init();
};

Calibrater::~Calibrater()
{
};

void Calibrater::init() {
    gyro_bias.setvalues(0.0f, 0.0f, 0.0f);
    accel_bias.setvalues(0.0f, 0.0f, 0.0f);
    calibrated = false;
};

void Calibrater::update(float gx, float gy, float gz, float ax, float ay, float az) {
    sensor_state.setvalues(gx, gy, gz, ax, ay, az);
};

void Calibrater::calibrate() {
    if (calibrated) {
        apply_gyro_calibration(&gyro_bias, &sensor_state);
        apply_accel_calibration(&accel_bias, &sensor_state);
        return; // すでにキャリブレーション済み
    }
    calibrate_gyro(&sensor_state, &gyro_bias);
    calibrate_accel(&sensor_state, &accel_bias);
    calibrated = true;
};

void Calibrater::calibrate_gyro(const SensorState* state, SensorBias* gyro_bias) {
    const int sample_count = 1000;
    float sum_x = 0, sum_y = 0, sum_z = 0;

    for (int i = 0; i < sample_count; i++) {
        sum_x += state->gyro_x;
        sum_y += state->gyro_y; 
        sum_z += state->gyro_z;
    }

    gyro_bias->x = sum_x / sample_count;
    gyro_bias->y = sum_y / sample_count;
    gyro_bias->z = sum_z / sample_count;
};

void Calibrater::calibrate_accel(const SensorState* state, SensorBias* accel_bias) {
    const int sample_count = 1000;
    float sum_x = 0, sum_y = 0, sum_z = 0;
    
    for (int i = 0; i < sample_count; i++) {
        sum_x += state->accel_x;
        sum_y += state->accel_y;
        sum_z += state->accel_z;
    }
    
    accel_bias->x = sum_x / sample_count;
    accel_bias->y = sum_y / sample_count;
    accel_bias->z = (sum_z / sample_count) - 9.81f; // 重力加速度を考慮
};

void Calibrater::apply_gyro_calibration(const SensorBias* gyro_bias, SensorState* state) {
    state->gyro_x -= gyro_bias->x;
    state->gyro_y -= gyro_bias->y;
    state->gyro_z -= gyro_bias->z;
};

void Calibrater::apply_accel_calibration(const SensorBias* accel_bias, SensorState* state) {
    state->accel_x -= accel_bias->x;
    state->accel_y -= accel_bias->y;
    state->accel_z -= accel_bias->z;
};

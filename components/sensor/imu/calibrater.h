#ifndef CALIBRATER_H
#define CALIBRATER_H

#include <math.h>

class Calibrater
{
    public:
        // センサーの状態を表す構造体
        typedef struct {
            float gyro_x;   // ジャイロスコープ X 軸
            float gyro_y;   // ジャイロスコープ Y 軸
            float gyro_z;   // ジャイロスコープ Z 軸
            float accel_x;  // 加速度センサー X 軸
            float accel_y;  // 加速度センサー Y 軸
            float accel_z;  // 加速度センサー Z 軸
            void setvalues(float gx, float gy, float gz, float ax, float ay, float az) {
                float samplerate = 100.0f; // サンプリング周波数
                float freq = 10.0f; // カットオフ周波数
                float q = 0.707f; // フィルタのQ値
                float omega = 2.0f * 3.14159265f *  freq/samplerate;
                float alpha = sin(omega) / (2.0f * q);
                gyro_x = gx;
                gyro_y = gy;
                gyro_z = gz;
                accel_x = ax;
                accel_y = ay;
                accel_z = az;
                lowpass_filter(alpha);
            }
            void lowpass_filter(float alpha) {
                gyro_x = alpha * gyro_x + (1 - alpha) * gyro_x;
                gyro_y = alpha * gyro_y + (1 - alpha) * gyro_y;
                gyro_z = alpha * gyro_z + (1 - alpha) * gyro_z;
                accel_x = alpha * accel_x + (1 - alpha) * accel_x;
                accel_y = alpha * accel_y + (1 - alpha) * accel_y;
                accel_z = alpha * accel_z + (1 - alpha) * accel_z;
            }
        } SensorState;

        // センサーのバイアスを表す構造体
        typedef struct {
            float x;   // バイアス X 軸
            float y;   // バイアス Y 軸
            float z;   // バイアス Z 軸
            void setvalues(float bx, float by, float bz) {
                x = bx;
                y = by;
                z = bz;
            }
        } SensorBias;

        void init();
        void update(float gx, float gy, float gz, float ax, float ay, float az);
        void calibrate();
        const SensorState& get_sensor_state() const { return sensor_state; }
        const SensorBias& get_gyro_bias() const { return gyro_bias; }
        const SensorBias& get_accel_bias() const { return accel_bias; }
        private:
        SensorState sensor_state;
        SensorBias gyro_bias, accel_bias;
        bool calibrated;
        void calibrate_gyro(const SensorState* state, SensorBias* gyro_bias);
        void calibrate_accel(const SensorState* state, SensorBias* accel_bias);
        void apply_gyro_calibration(const SensorBias* gyro_bias, SensorState* state);
        void apply_accel_calibration(const SensorBias* accel_bias, SensorState* state);
    public:
        Calibrater();
        ~Calibrater();


};

#endif // CALIBRATER_H

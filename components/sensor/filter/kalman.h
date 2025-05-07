#ifndef KALMAN_H
#define KALMAN_H

#define sampleFreqDef   100.0f          // sample frequency in Hz

#include <math.h>
#include <stdio.h>
#include <algorithm>
#include "eulerAngle.h"

class Kalman{
    public:
        typedef struct {
            float gyro_x;   // ジャイロスコープ X 軸
            float gyro_y;   // ジャイロスコープ Y 軸
            float gyro_z;   // ジャイロスコープ Z 軸
            float accel_x;  // 加速度センサー X 軸
            float accel_y;  // 加速度センサー Y 軸
            float accel_z;  // 加速度センサー Z 軸
            void setvalues(float gx, float gy, float gz, float ax, float ay, float az) {
                gyro_x = gx;
                gyro_y = gy;
                gyro_z = gz;
                accel_x = ax;
                accel_y = ay;
                accel_z = az;
            }
        } SensorState;

        // カルマンフィルタの状態を表す構造体
        typedef struct {
            float angle;    // フィルタされた角度
            float bias;     // ジャイロのバイアス
            float rate;     // ジャイロの角速度
            float P[2][2];  // 誤差共分散行列
        } KalmanFilter;

    public:
        Kalman();
        ~Kalman();
        public:
            void init();
            void begin(float sampleFrequency) { invSampleFreq = 1.0f / sampleFrequency; }
            void update(float gx, float gy, float gz, float ax, float ay, float az);
            float getPitch();
            float getRoll();
            float getYaw();
        private:
            KalmanFilter kalman_pitch, kalman_roll;
            SensorState state;
            float invSampleFreq; // フィルタのsampling周波数
            bool onEase = false; // イージングを適用するかどうか

            void kalman_init(KalmanFilter* kf);
            float kalman_update(KalmanFilter* kf, float new_angle, float new_rate, float dt);
            void calculate_orientation(const SensorState* state, EulerAngle* orientation, float delta_time);
            void calculate_position(const SensorState* state, float* velocity, float* position, float delta_time);
            void correct_gravity(const SensorState* state, float* accel_corrected, const EulerAngle* orientation);

            float compulemant_filter(float gyro, float accel, float angle, float alpha, float dt);
            float ease(float current, float target, float factor);
        private:
            EulerAngle orientation = {0.0f, 0.0f, 0.0f, EulerOrder::ZYX};
            float smoothed_pitch = 0.0f;
            float smoothed_roll = 0.0f;
        
            float easing_factor = 0.05f; // イージング係数（小さいほど変化が滑らか）
};

#endif // KALMAN_H
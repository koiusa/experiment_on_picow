#ifndef BMI055_H
#define BMI055_H

#include <iostream>
#include <unordered_map>

/// @brief BMI055センサーのクラス
/// @details BMI055センサーのクラスで、加速度センサーとジャイロセンサーの分解能を取得するためのメソッドを提供します。
class BMI055 {
    public:
        // sensitivity | °/s Gyroのサンプリング周波数
        enum class ACCEL_RATE {
            G2, G4, G8, G16
        };
        // sensitivity / g Accelのサンプリング周波数F
        enum class GYRO_RATE {
            RS125, RS250, RS500, RS1000, RS2000
        };
    private:
        static constexpr float GYRO_SENSITIVITY = 32767.0f;
        static constexpr float ACCEL_SENSITIVITY = 1024.0f;
        std::unordered_map<ACCEL_RATE, float> accel_rate = {
            {ACCEL_RATE::G2, BMI055::ACCEL_SENSITIVITY / 2.0f},
            {ACCEL_RATE::G4,  BMI055::ACCEL_SENSITIVITY / 4.0f},
            {ACCEL_RATE::G8, BMI055::ACCEL_SENSITIVITY / 8.0f},
            {ACCEL_RATE::G16, BMI055::ACCEL_SENSITIVITY / 16.0f}
        };
        std::unordered_map<GYRO_RATE, float> gyro_rate = {
            {GYRO_RATE::RS125, BMI055::GYRO_SENSITIVITY / 125.0f},
            {GYRO_RATE::RS250, BMI055::GYRO_SENSITIVITY / 250.0f},
            {GYRO_RATE::RS500, BMI055::GYRO_SENSITIVITY / 500.0f},
            {GYRO_RATE::RS1000, BMI055::GYRO_SENSITIVITY / 1000.0f},
            {GYRO_RATE::RS2000, BMI055::GYRO_SENSITIVITY / 2000.0f}
        };
    public:
        ACCEL_RATE accel_rate_type;
        GYRO_RATE gyro_rate_type;
        float get_accel_rate_value() {        
            return round(this->accel_rate[accel_rate_type] * 10.0f) / 10.f; // 10.0fはスケーリングファクター
        };
        float get_gyro_rate_value() {
            return round(this->gyro_rate[gyro_rate_type]* 10.0f) / 10.f;
        };
        int get_accel_rate_type_count() {
            return accel_rate.size();
        };
        int get_gyro_rate_type_count() {
            return gyro_rate.size();
        };
};

#endif // BMI055_H

#ifndef dUMMY_H
#define dUMMY_H 

#include "kalman.h"
#include "MadgwickAHRS.h"
#include "calibrater.h"
#include "quaternion.h"
#include "serialplot.h"
#include "ds4_on_pico_w.hpp"
#include "BMI055.h"
#include "picow_udp.h"
#include "logic.h"
#include <chrono>
#include <algorithm>

#include "picow_udp.h"
#include "Ids4state.h"

class DS4ImuTester : public PicowUDP::IUdpListener, public IDs4state {
    // デバウンス処理のためのクラス
    // ボタンの状態を保存し、一定時間が経過した後に有効な状態かどうかを判断する
    class debounce {
        private:
            bool previous_state = false;
            std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
            std::chrono::steady_clock::time_point last_press_time = std::chrono::steady_clock::now();
        public:
            // ボタンが押された時間を保存
            void pressed() {
                last_press_time = std::chrono::steady_clock::now();
            }
            // ボタンの状態を保存
            bool isNotBounce() {
                now = std::chrono::steady_clock::now();
                return std::chrono::duration_cast<std::chrono::milliseconds>(now - last_press_time).count() > 200; // 200msのデバウンス
            }
            bool isValid(bool othor_state) {
                return !previous_state && othor_state; // 現在の状態が前回の状態と異なる場合
            }
            void saveState(bool current_state) {
                previous_state = current_state; // 現在の状態を保存
            }
    };

    private:
        Calibrater calibrater;
        float gyro_rate; // Get the gyro resolution
        float accel_rate; // Get the accel resolution
        
        Kalman kalman; // Kalmanフィルタのインスタンス
        
        Madgwick madgwick; // Madgwickフィルタのインスタンス
        BMI055 imu_sensor; // IMUセンサーのインスタンス
        int rate_target = 0; // ターゲットセンサーのインデックス
        
        EulerAngle manual = {0, 0, 0, EulerOrder::ZYX}; // 現在のオリエンテーション

        debounce debounce_order;
        debounce debounce_rezolusion;
        debounce debounce_rate_target;

        float freq = 100.0f; // フィルタのsampling周波数

        EulerAngle yrp = {0, 0, 0, EulerOrder::ZYX }; // Yaw, Roll, Pitch

    public:
        DS4ImuTester();
        ~DS4ImuTester();

        void update();
        void reset();
        void attach(PicowUDP* udp) override { this->udp = udp; };
        void attach(const DualShock4_state* state) override { this->state = state; };
    private:
        void kalman_update();
        void madgwick_update();
        void raw_update();
        void plot_update();
        void manual_update();
        void sensor_calibrate();
        void sensor_rezolution();
        void sensor_freaquency();
        void add_frequency(float freq);
        void set_rotate_order();
        void sensor_target();
        void save_previous_input();
};

#endif // DUMMY_H

#include "dummyinput.h"


Dummyinput::Dummyinput() {
    calibrater.init(); // Initialize the calibrater
    kalman.init(); // Initialize the Kalman filter
    kalman.begin(this->freq); // Initialize the Kalman filter with the default frequency
    madgwick.begin(this->freq); //フィルタのサンプリングを100Hz;
    imu_sensor.accel_rate_type = BMI055::ACCEL_RATE::G2; // 加速度センサーの分解能を設定
    imu_sensor.gyro_rate_type = BMI055::GYRO_RATE::RS125; // ジャイロセンサーの分解能を設定
};

Dummyinput::~Dummyinput() {
    // Destructor
    // Cleanup if necessary
};

void Dummyinput::update() {
    reset(); // Reset the state if necessary 
    serialplot::flush(state); // Flush the state to serial port
    serialplot::plot(state); // Plot a sinus 
    sensor_rezolution(); // Update the sensor resolution
    sensor_freaquency(); // Update the sensor frequency
    sensor_calibrate();
    set_rotate_order(); // Set the rotation order
    kalman_update(); // Update the Kalman filter
    madgwick_update(); // Update the Madgwick filter
    manual_update(); // Update the manual orientation
    save_previous_input(); // Save the previous state
};

void Dummyinput::set_State(const DualShock4_state state) {
    this->state = state;
};

void Dummyinput::reset() {
    if (state.options != 0) {
        // If the options button is pressed, reset the Kalman filter
        calibrater.init();
        kalman.init(); // Initialize the Kalman filter
    }
    if (state.l3 != 0) {
        manual.setvalues(0.0f, 0.0f, 0.0f); // Reset the orientation
    }
};

void Dummyinput::save_previous_input() {
    // Save the previous state
    debounce_order.saveState(state.r3 != 0); // Save the state of the right stick button
    debounce_rezolusion.saveState(state.hat == 0 || state.hat == 4); // Save the state of the circle button
    debounce_rate_target.saveState(state.hat == 2 || state.hat == 6); // Save the state of the hat switch
};

void Dummyinput::sensor_calibrate() {
    calibrater.update(state.gyro_x, state.gyro_y, state.gyro_z, state.accel_x, state.accel_y, state.accel_z);
    calibrater.calibrate(); // Calibrate the sensor
    Calibrater::SensorState sensor_state = calibrater.get_sensor_state(); // Get the sensor state
    printf(">calibrate_Gyro_x:%f|np\n", sensor_state.gyro_x / gyro_rate);
    printf(">calibrate_Gyro_y:%f|np\n", sensor_state.gyro_y / gyro_rate);
    printf(">calibrate_Gyro_z:%f|np\n", sensor_state.gyro_z / gyro_rate);
    printf(">calibrate_Accel_x:%f|np\n", sensor_state.accel_x / accel_rate);
    printf(">calibrate_Accel_y:%f|np\n", sensor_state.accel_y / accel_rate);
    printf(">calibrate_Accel_z:%f|np\n", sensor_state.accel_z / accel_rate);
};

void Dummyinput::sensor_freaquency() {
    if (state.circle != 0 && (state.l2_value != 0 || state.r2_value != 0)) {
        // If the circle button is pressed, add the frequency to the Kalman filter
        add_frequency((-state.l2_value + state.r2_value)/100.0f);
    }
    kalman.begin(this->freq); // Set the sampling frequency for the Kalman filter
    madgwick.begin(this->freq); // Set the sampling frequency for the Madgwick filter
    printf(">operate_filter_freq:%f|np\n", this->freq);
};

void Dummyinput::add_frequency(float freq) {
    this->freq += freq;
    this->freq = std::clamp(this->freq, 1.0f, 200.0f); // サンプリング周波数の範囲を制限
};

/// @brief Dummyinput::sensor_target
/// @details センサーのターゲットを設定します。
void Dummyinput::sensor_rezolution() {
    {
        // Set the target sensor based on the current state
        if (debounce_rate_target.isValid((state.hat == 2 || state.hat == 6) && state.circle != 0)) { // 現在の状態が前回の状態と異なる場合
            if (debounce_rate_target.isNotBounce()) { // 200msのデバウンス
                rate_target = (state.hat == 2) ? 1 : 0; // ターゲットセンサーを設定
                debounce_rate_target.pressed(); // 最後に押された時間を更新
            }
        }
        printf(">operate_rate_target:%3d|np\n", rate_target);
    }
    {
        // Set the sensor resolution based on the current state
        if (debounce_rezolusion.isValid((state.hat == 0 || state.hat == 4) && state.circle != 0)) { // 現在の状態が前回の状態と異なる場合
            if (debounce_rezolusion.isNotBounce()) { // 200msのデバウンス
                int direction = (state.hat == 0) ? 1 : -1;
                switch (rate_target) {
                    case 0: {
                        int cnt = imu_sensor.get_accel_rate_type_count();
                        int next = static_cast<int>(imu_sensor.accel_rate_type) + direction;
                        next = next >= 0 ? next : cnt - 1;  
                        imu_sensor.accel_rate_type = static_cast<BMI055::ACCEL_RATE>(next % cnt);
                        break;}
                    case 1:{
                        int cnt = imu_sensor.get_gyro_rate_type_count();
                        int next = static_cast<int>(imu_sensor.gyro_rate_type) + direction;
                        next = next >= 0 ? next : cnt - 1;
                        imu_sensor.gyro_rate_type = static_cast<BMI055::GYRO_RATE>(next % cnt);
                        break;}
                }
                debounce_rezolusion.pressed(); // 最後に押された時間を更新
            }
        }
        printf(">operate_accel_rate_type:%3d|np\n", static_cast<int>(imu_sensor.accel_rate_type));
        printf(">operate_gyro_rate_type:%3d|np\n", static_cast<int>(imu_sensor.gyro_rate_type));
    }
    gyro_rate = imu_sensor.get_gyro_rate_value(); // Get the gyro rate
    accel_rate = imu_sensor.get_accel_rate_value(); // Get the accel rate
    printf(">operate_gyro_rate:%f|np\n", gyro_rate);
    printf(">operate_accel_rate:%f|np\n", accel_rate);
};

/// @brief Dummyinput::kalman_update
/// @details Kalmanフィルタを使用して、センサーの状態を更新します。
void Dummyinput::kalman_update() {
    Calibrater::SensorState sensor_state = calibrater.get_sensor_state(); // Get the sensor state
    kalman.update(sensor_state.gyro_x / gyro_rate, sensor_state.gyro_y / gyro_rate, sensor_state.gyro_z / gyro_rate,
                    sensor_state.accel_x / accel_rate, sensor_state.accel_y / accel_rate, sensor_state.accel_z / accel_rate); // Calculate the orientation
    float pitch = kalman.getPitch();
    float roll = kalman.getRoll();
    float yaw = kalman.getYaw();
    EulerAngle euler = {pitch, roll, yaw, manual.order}; // Create an Euler angle object
    Quaternion qt = Quaternion::toQuaternion(euler.toRadians()); // Convert to quaternion
    printf(">3D|kalman:S:cube:P:0:0:0:Q:%f:%f:%f:%f:W:8:H:3:D:1:C:red\n", qt.x, qt.y, qt.z,qt.w);
    printf(">3D|kalman_euler:S:cube:P:0:0:0:R:%f:%f:%f:W:8:H:3:D:1:C:red|np\n", euler.x, euler.y, euler.z);
    printf(">kalman_pitch:%f|np\n", euler.x);
    printf(">kalman_roll:%f|np\n", euler.y);
    printf(">kalman_yaw:%f|np\n", euler.z);
    printf(">kalman_q_x:%f|np\n", qt.x);
    printf(">kalman_q_y:%f|np\n", qt.y);
    printf(">kalman_q_z:%f|np\n", qt.z);
    printf(">kalman_q_w:%f|np\n", qt.w);
};

/// @brief Dummyinput::madgwick_update
/// @details Madgwickフィルタを使用して、センサーの状態を更新します。
void Dummyinput::madgwick_update() {
    Calibrater::SensorState sensor_state = calibrater.get_sensor_state(); // Get the sensor state
    madgwick.updateIMU(sensor_state.gyro_x / gyro_rate, sensor_state.gyro_y / gyro_rate, sensor_state.gyro_z / gyro_rate,
                    sensor_state.accel_x / accel_rate, sensor_state.accel_y / accel_rate, sensor_state.accel_z / accel_rate); // Update the Madgwick filter
    float pitch = madgwick.getPitch();
    float roll = madgwick.getRoll();
    float yaw = madgwick.getYaw();
    EulerAngle euler = {pitch, roll, yaw, manual.order}; // Create an Euler angle object
    Quaternion qt = Quaternion::toQuaternion(euler.toRadians()); // Convert to quaternion
    printf(">3D|madgwick:S:cube:P:0:0:0:Q:%f:%f:%f:%f:W:8:H:3:D:1:C:red\n", qt.x, qt.y, qt.z,qt.w);
    printf(">3D|madgwick_euler:S:cube:P:0:0:0:R:%f:%f:%f:W:8:H:3:D:1:C:red|np\n", euler.x, euler.y, euler.z);
    printf(">madgwick_pitch:%f|np\n", euler.x);
    printf(">madgwick_roll:%f|np\n", euler.y);
    printf(">madgwick_yaw:%f|np\n", euler.z);
    printf(">madgwick_q_x:%f|np\n", qt.x);
    printf(">madgwick_q_y:%f|np\n", qt.y);
    printf(">madgwick_q_z:%f|np\n", qt.z);
    printf(">madgwick_q_w:%f|np\n", qt.w);
};

/// @brief Dummyinput::manual_update
/// @details 手動でオリエンテーションを更新します。
void Dummyinput::manual_update() {
    float magnification = 10.0f; // Magnification factor
    float pad_x = ((state.l3_x / 255.0f) - 0.5f) * magnification; // Normalize the values
    float pad_y = ((state.l3_y / 255.0f) - 0.5f) * magnification; // Normalize the values
    float trigger_l = (state.l2_value / 255.0f) * magnification; // Normalize the values
    float trigger_r = (state.r2_value / 255.0f) * magnification; // Normalize the values
    float trigger = (-trigger_l + trigger_r) / 2; // Average the trigger values
    if (pad_x != 0 || pad_y != 0 || trigger != 0) {
        EulerAngle operate = {pad_x, pad_y, trigger, manual.order}; // Create an Euler angle object
        this->manual.x += operate.x;
        this->manual.y += operate.y;
        this->manual.z += operate.z;
    }
    Quaternion qt = Quaternion::toQuaternion(manual.toRadians()); // Convert to quaternion
    printf(">3D|manual:S:cube:P:0:0:0:Q:%f:%f:%f:%f:W:8:H:3:D:1:C:red\n", qt.x, qt.y, qt.z,qt.w);
    printf(">3D|manual_euler:S:cube:P:0:0:0:R:%f:%f:%f:W:8:H:3:D:1:C:red|np\n", manual.x, manual.y, manual.z);
    printf(">manual_pitch:%f|np\n", manual.x);
    printf(">manual_roll:%f|np\n", manual.y);
    printf(">manual_yaw:%f|np\n", manual.z);
    printf(">manual_q_x:%f|np\n", qt.x);
    printf(">manual_q_y:%f|np\n", qt.y);
    printf(">manual_q_z:%f|np\n", qt.z);
    printf(">manual_q_w:%f|np\n", qt.w);

    Calibrater::SensorState sensor_state = calibrater.get_sensor_state(); // Get the sensor state
    float timeStep = 1.0f / this->freq; // Calculate the time step
    yrp.x += sensor_state.gyro_x / gyro_rate * timeStep; // Update the pitch
    yrp.y += sensor_state.gyro_y / gyro_rate * timeStep;
    yrp.z += sensor_state.gyro_z / gyro_rate * timeStep;
    yrp.order = manual.order; // Set the rotation order
    Quaternion yrp_qt = Quaternion::toQuaternion(yrp.toRadians());
    printf(">3D|raw:S:cube:P:0:0:0:Q:%f:%f:%f:%f:W:8:H:3:D:1:C:red\n", yrp_qt.x, yrp_qt.y, yrp_qt.z, yrp_qt.w);
    printf(">3D|raw_euler:S:cube:P:0:0:0:R:%f:%f:%f:W:8:H:3:D:1:C:red|np\n", yrp.x, yrp.y, yrp.z);
};

void Dummyinput::set_rotate_order() {
    if (debounce_order.isValid(state.r3 != 0)) { // Check if the right stick button is pressed
        if (debounce_order.isNotBounce()) { // 200msのデバウンス
            manual.order = static_cast<EulerOrder>((static_cast<int>(manual.order) + 1) % 6); // Change the rotation order
            debounce_order.pressed(); // Update the last press time
        }
    }
    printf(">operate_rotation_order:%d|np\n", static_cast<int>(manual.order));
};

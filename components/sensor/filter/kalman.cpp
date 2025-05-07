#include "kalman.h"

// コンストラクタ
Kalman::Kalman() {
    // 必要な初期化処理を記述
    begin(sampleFreqDef); // デフォルトのサンプリング周波数で初期化
};

// デストラクタ
Kalman::~Kalman() {
    // 必要なクリーンアップ処理を記述
};

// カルマンフィルタの初期化
void Kalman::kalman_init(KalmanFilter* kf) {
    kf->angle = 0.0f;
    kf->bias = 0.0f;
    kf->rate = 0.0f;
    kf->P[0][0] = 1.0f;
    kf->P[0][1] = 0.0f;
    kf->P[1][0] = 0.0f;
    kf->P[1][1] = 1.0f;
};

// カルマンフィルタの更新
float Kalman::kalman_update(KalmanFilter* kf, float new_angle, float new_rate, float dt) {
    // 予測ステップ
    kf->rate = new_rate - kf->bias;
    kf->angle += dt * kf->rate;

    kf->P[0][0] += dt * (dt * kf->P[1][1] - kf->P[0][1] - kf->P[1][0] + 0.001f);
    kf->P[0][1] -= dt * kf->P[1][1];
    kf->P[1][0] -= dt * kf->P[1][1];
    kf->P[1][1] += 0.003f;

    // 更新ステップ
    float S = kf->P[0][0] + 0.03f;
    float K[2];
    K[0] = kf->P[0][0] / S;
    K[1] = kf->P[1][0] / S;

    float y = new_angle - kf->angle;
    kf->angle += K[0] * y;
    kf->bias += K[1] * y;

    float P00_temp = kf->P[0][0];
    float P01_temp = kf->P[0][1];

    kf->P[0][0] -= K[0] * P00_temp;
    kf->P[0][1] -= K[0] * P01_temp;
    kf->P[1][0] -= K[1] * P00_temp;
    kf->P[1][1] -= K[1] * P01_temp;

    return kf->angle;
};

float Kalman::ease(float current_value, float target_value, float easing_factor) {
    return current_value + (target_value - current_value) * easing_factor;
}

float Kalman::compulemant_filter(float gyro_angle, float accel_angle, float previous_angle, float alpha, float delta_time) {
    float gyro_contribution = previous_angle + gyro_angle * delta_time; // ジャイロの値を積分
    return (alpha * gyro_contribution) + ((1.0f - alpha) * accel_angle);
}

float Kalman::getPitch() {
    return orientation.x;
};

float Kalman::getRoll() {
    return orientation.y;
};

float Kalman::getYaw() {
    return orientation.z;
};

// 姿勢を計算する関数
void Kalman::calculate_orientation(const SensorState* state, EulerAngle* orientation, float delta_time) {
    // 加速度センサーからピッチとロールを計算
    float accel_pitch = atan2f(-state->accel_x, sqrtf(state->accel_y * state->accel_y + state->accel_z * state->accel_z)) * 180.0f / M_PI;
    float accel_roll = atan2f(state->accel_y, state->accel_z) * 180.0f / M_PI;

    float alpha = 0.98f; // フィルタ係数（ジャイロの信頼度）
    float calc_pitch = compulemant_filter(state->gyro_x, accel_pitch, orientation->x, alpha, delta_time);
    float calc_roll = compulemant_filter(state->gyro_y, accel_roll, orientation->y, alpha, delta_time);
    
    if (onEase) {
        // イージングを適用する場合
        smoothed_pitch = ease(smoothed_pitch, calc_pitch, easing_factor);
        smoothed_roll = ease(smoothed_roll, calc_roll, easing_factor);
    } else {
        // イージングを適用しない場合
        smoothed_pitch = accel_pitch;
        smoothed_roll = accel_roll;
    }

    // ジャイロスコープの角速度を使用してカルマンフィルタを更新
    orientation->x = kalman_update(&kalman_pitch, smoothed_pitch, state->gyro_x, delta_time);
    orientation->y = kalman_update(&kalman_roll, smoothed_roll, state->gyro_y, delta_time);
    
    // ヨーをジャイロスコープから積分して計算
    orientation->z += state->gyro_z * delta_time; // ジャイロのZ軸値を積分
};

// 加速度から速度と位置を計算
void Kalman::calculate_position(const SensorState* state, float* velocity, float* position, float delta_time) {
    // 重力補正（姿勢を考慮して重力成分を除去する必要あり）
    float accel_x_corrected = state->accel_x;
    float accel_y_corrected = state->accel_y;
    float accel_z_corrected = state->accel_z - 9.81f; // 重力加速度を引く

    // 速度を更新
    velocity[0] += accel_x_corrected * delta_time;
    velocity[1] += accel_y_corrected * delta_time;
    velocity[2] += accel_z_corrected * delta_time;

    // 位置を更新
    position[0] += velocity[0] * delta_time;
    position[1] += velocity[1] * delta_time;
    position[2] += velocity[2] * delta_time;
};

// 重力補正
void Kalman::correct_gravity(const SensorState* state, float* accel_corrected, const EulerAngle* orientation) {
    float gravity_x = sinf(orientation->x * M_PI / 180.0f) * 9.81f;
    float gravity_y = -sinf(orientation->y * M_PI / 180.0f) * 9.81f;
    float gravity_z = cosf(orientation->x * M_PI / 180.0f) * cosf(orientation->y * M_PI / 180.0f) * 9.81f;

    accel_corrected[0] = state->accel_x - gravity_x;
    accel_corrected[1] = state->accel_y - gravity_y;
    accel_corrected[2] = state->accel_z - gravity_z;
};

void Kalman::init() {
    // カルマンフィルタの初期化
    kalman_init(&kalman_pitch);
    kalman_init(&kalman_roll);
};

// テスト用のメイン関数
void Kalman::update(float gx, float gy, float gz, float ax, float ay, float az) {
    state.setvalues(gx, gy, gz, ax, ay, az); // センサーの状態を更新
    float delta_time = this->invSampleFreq; // サンプリング周期（例: 10ms）
    calculate_orientation(&state, &orientation, delta_time);
};

#include "servo_drive_tester.h"



void ServoDriveTester::update()
{
    if (debounce_reset.isValid(state->triangle != 0)) { // 現在の状態が前回の状態と異なる場合
        if (debounce_reset.isNotBounce()) { // 200msのデバウンス
            begin();
            debounce_reset.pressed(); // 最後に押された時間を更新
        }
    }

    // スティック入力を正規化
    float norm_x = logic::remap(state->r3_x, 1.0f, 255.0f, -1.0f, 1.0f);
    float norm_y = logic::remap(state->r3_y, 1.0f, 255.0f, -1.0f, 1.0f);

    // 単位円内にクリッピング
    // Eigenベクトルで扱う
    Eigen::Vector2f stick(norm_x, norm_y);
   float length = stick.norm();
    if (length > 1.0f) {
        stick /= length;
        length = 1.0f;
    }

//    // 前フレームのスティックベクトル
//     Eigen::Vector2f prev_stick(prev_stick_x, prev_stick_y);
//     float prev_length = prev_stick.norm();
//     float dot = stick.dot(prev_stick);

//     // スティックがニュートラルに戻る方向ならサーボを動かさない
//     constexpr float STICK_EPSILON = 1e-3f;
//     bool is_returning_to_neutral = (dot < 0) && (length > STICK_EPSILON) && (prev_length > STICK_EPSILON);

//     // 加算値として使う（例：スケール値は任意で調整）
//     float add_y = !is_returning_to_neutral ? (norm_y * -10.0f) : 0.0f;
//     float add_x = !is_returning_to_neutral ? (norm_x * 10.0f) : 0.0f;

    float add_y =  (norm_y * -10.0f);
    float add_x =  (norm_x * 10.0f) ;

    // 最大値も単位球でスケーリング
    float max_y = 90.0f;
    float min_y = 0.0f;
    float max_z = 90.0f;
    float min_z = -90.0f;

    drive(servo_y, add_y, min_y, max_y);
    drive(servo_z, add_x, min_z, max_z);
    
    printf("%s [R3] %f:%f\n", "SERVO" , servo_y.get_current_angle(), servo_z.get_current_angle());
    osc::bundle angle{osc::time()};
    angle << (osc::message{ "/servo" } << float(servo_y.get_current_angle()) << float(servo_z.get_current_angle()));
    udp->send_bundle(angle); // Add the message to the UDP queue

    prev_stick_y = stick.y();
    prev_stick_x = stick.x();
    debounce_reset.saveState(state->triangle != 0); // Save the state of the triangle button
};

void ServoDriveTester::drive(sg90& servo ,float input_value, float min_angle, float max_angle) {
    float goal_angle = std::clamp(servo.get_current_angle() + input_value, min_angle, max_angle); // Reduce the sensitivity of the left stick
    servo.drive_to_angle(goal_angle);
    constexpr float EPSILON = 1e-1f; // 許容誤差
    if (std::abs(input_value) > EPSILON) {
        servo.apply_angle(true);
    }else {
        servo.apply_angle(false);
    }    
};


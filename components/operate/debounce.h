#ifndef DEBOUNCE_H
#define DEBOUNCE_H

#include <chrono>

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

#endif // DEBOUNCE_H

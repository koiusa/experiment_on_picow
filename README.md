# DualShock4 on Pico-W

## 概要

[usedbytes/picow_ds4](https://github.com/usedbytes/picow_ds4)を元に、VS-Codeの拡張機能「raspberry-pi.raspberry-pi-pico」を使用して、Pico-WとDualShock4を接続するためのサンプルプログラムを作成しました。



## 取り込み方

### CMakeLists.txt

自プログラムに取り込む場合は、プロジェクトを配置後に「CMakeLists.txt」に下記のことを記載してください。

```cmake
add_subdirectory(DualShock4_on_picow/lib)

# Add any user requested libraries
target_link_libraries(DualShock4_on_picow
    lib_ds4_on_pico
)
```

### Sample code

Sample code is as follows:

```cpp
#include "ds4_on_pico_w.h"

int your_function()
{
    DS4forPicoW controller;
    bool loop_contents = true;
    do {
        printf("Starting inquiry scan..\n");
        loop_contents = controller.scan();
    } while (false == loop_contents);

    while (loop_contents) {
       bt_hid_state state = controller.get_state();
    }
}
```


## 構造体

取得する構造体"DualShock4_state"の説明


| グループ             | 定義     | 構造体      | 概要                                       |
| -------------------- | -------- | ----------- | ------------------------------------------ |
| 十字キー             | uint8_t  | hat         | 8は未入力、Up=1として45度づつ、+1 されます |
| メニューボタン       | bool     | share       | PADの左側にあるメニューボタン              |
| ^                    | bool     | options     | PADの右側にあるメニューボタン              |
| ^                    | bool     | ps          | PADの下側にあるメニューボタン              |
| アクション ボタン    | bool     | triangle    | △ ボタン                                   |
| ^                    | bool     | square      | □ ボタン                                   |
| ^                    | bool     | circle      | ○ ボタン                                   |
| ^                    | bool     | cross       | × ボタン                                   |
| トリガーボタン(左)   | bool     | l1          | 上面のL1 ボタン                            |
| ^                    | bool     | l2          | 上面のL2 ボタン                            |
| ^                    | uint16_t | l2_value    | 上面のL2 ボタン 押し込み量(0～255)         |
| ^                    | bool     | l3          | 左側のステック ボタン 押し込み時           |
| ^                    | uint16_t | l3_x        | 左側のステック X軸                         |
| ^                    | uint16_t | l3_y        | 左側のステック Y軸                         |
| トリガーボタン  (右) | bool     | r1          | 上面のR1 ボタン                            |
| ^                    | bool     | r2          | 上面のR2 ボタン                            |
| ^                    | uint16_t | r2_value    | 上面のR2 ボタン 押し込み量(0～255)         |
| ^                    | bool     | r3          | 右側のステック ボタン 押し込み時           |
| ^                    | uint16_t | r3_x        | 右側のステック X軸                         |
| ^                    | uint16_t | r3_y        | 右側のステック Y軸                         |
| パッド               | bool     | touch       | タッチパッドのクリック時                   |
| ^                    | uint8_t  | pad_x       | [確認中]                                   |
| ^                    | uint8_t  | pad_y       | [確認中]                                   |
| ジャイロセンサー     | int16_t  | gyro_x      | [確認中]                                   |
| ^                    | int16_t  | gyro_y      | [確認中]                                   |
| ^                    | int16_t  | gyro_z      | [確認中]                                   |
| 加速度センサー       | int16_t  | accel_x     | [確認中]                                   |
| ^                    | int16_t  | accel_y     | [確認中]                                   |
| ^                    | int16_t  | accel_z     | [確認中]                                   |
| バッテリーレベル     | uint16_t | battery     | [確認中] Lv1～3かな                        |
| 温度                 | uint16_t | temperature | [確認中]                                   |
| タイムスタンプ       | uint16_t | timestamp   | [確認中]                                   |
| ステータス           | bool     | connected   | 接続していた場合、true                     |




## Changelog

It is listed [here](./Changelog).

## Support

Kindly provide the details by creating a new issue.

## Contributing

We welcome pull requests from the community. If you're considering significant changes, we kindly ask you to begin by opening an issue to initiate a discussion about your proposed modifications.
Additionally, when submitting a pull request, please ensure that any relevant tests are updated or added as needed.


## Authors and acknowledgment

We offer heartfelt thanks to the open-source community for the invaluable gifts they've shared with us. The hardware, libraries, and tools they've provided have breathed life into our journey of development. Each line of code and innovation has woven a tapestry of brilliance, lighting our path. In this symphony of ingenuity, we find ourselves humbled and inspired. These offerings infuse our project with boundless possibilities. As we create, they guide us like stars, reminding us that collaboration can turn dreams into reality. With deep appreciation, we honor the open-source universe that nurtures us on this journey of discovery and growth.

## License

[BSD-3-Clause license](./LICENSE)

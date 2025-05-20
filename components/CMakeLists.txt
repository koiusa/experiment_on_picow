add_library(components_on_picow STATIC)

# 共通処理のライブラリを追加
add_subdirectory(logic)
target_link_libraries(components_on_picow
    lib_logic
)

# OSC通信用のライブラリを追加
add_subdirectory(network/OSC)
target_link_libraries(components_on_picow
    lib_osc
)

# Wi-Fi用のライブラリを追加
add_subdirectory(network/UDP)
target_link_libraries(components_on_picow
    lib_picow_udp
)

# 行列計算用のライブラリを追加
add_subdirectory(math/matrix)
target_link_libraries(components_on_picow
    lib_matrix
)

# センサー用のライブラリを追加
add_subdirectory(sensor/filter)
target_link_libraries(components_on_picow
    lib_filter
)
# IMU用のライブラリを追加
add_subdirectory(sensor/imu)
target_link_libraries(components_on_picow
    lib_imu
)

# サーボモーター用のライブラリを追加
add_subdirectory(servo/sg90)
target_link_libraries(components_on_picow
    lib_sg90
)

# 入力補助用のライブラリを追加
add_subdirectory(operate)
target_link_libraries(components_on_picow
    lib_operate
)

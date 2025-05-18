add_library(lib_imu STATIC
    calibrater.cpp
)

target_include_directories(lib_imu PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")
# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(DualShock4_on_picow C CXX ASM)

set(PICO_CXX_ENABLE_EXCEPTIONS 1)

set(PICO_CXX_ENABLE_RTTI 1)

# Initialize the Raspberry Pi Pico SDK
add_definitions(-D PICO_FLASH_ASSERT_ON_UNSAFE=0)

pico_sdk_init()

add_subdirectory(lib)

# Add executable. Default name is the project name

add_executable(DualShock4_on_picow
  main.cpp
)

pico_set_program_name(DualShock4_on_picow "DualShock4_on_picow")
pico_set_program_version(DualShock4_on_picow "0.2.0")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(DualShock4_on_picow 0)
pico_enable_stdio_usb(DualShock4_on_picow 1)

# Add the standard library to the build
target_link_libraries(DualShock4_on_picow
    pico_stdlib
)

# Add the standard include files to the build
target_include_directories(DualShock4_on_picow PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(DualShock4_on_picow
    lib_ds4_on_pico
)

pico_add_extra_outputs(DualShock4_on_picow)

# プロット用のソースファイルを追加
include_directories(${CMAKE_SOURCE_DIR}/debug/plot)
file(GLOB PLOT_SOURCES ${CMAKE_SOURCE_DIR}/debug/plot/*.cpp)
target_sources(${PROJECT_NAME} PRIVATE ${PLOT_SOURCES})

# 入力テスト用のソースファイルを追加
include_directories(${CMAKE_SOURCE_DIR}/debug/operate)
file(GLOB PLOT_SOURCES ${CMAKE_SOURCE_DIR}/debug/operate/*.cpp)
target_sources(${PROJECT_NAME} PRIVATE ${PLOT_SOURCES})

# 行列計算用のソースファイルを追加
include_directories(${CMAKE_SOURCE_DIR}/components/math/matrix) 
file(GLOB MATRIX_SOURCES ${CMAKE_SOURCE_DIR}/components/math/matrix/*.cpp)
target_sources(${PROJECT_NAME} PRIVATE ${MATRIX_SOURCES})

include_directories(${CMAKE_SOURCE_DIR}/components/math/eigen) 
file(GLOB EIGEN_SOURCES ${CMAKE_SOURCE_DIR}/components/math/eigen/Eigen/src/*/*.cpp)
target_sources(${PROJECT_NAME} PRIVATE ${EIGEN_SOURCES})

# センサー用のソースファイルを追加
include_directories(${CMAKE_SOURCE_DIR}/components/sensor/filter)
file(GLOB SENSOR_SOURCES ${CMAKE_SOURCE_DIR}/components/sensor/filter/*.cpp)
target_sources(${PROJECT_NAME} PRIVATE ${SENSOR_SOURCES})

include_directories(${CMAKE_SOURCE_DIR}/components/sensor/imu)
file(GLOB IMU_SOURCES ${CMAKE_SOURCE_DIR}/components/sensor/imu/*.cpp)
target_sources(${PROJECT_NAME} PRIVATE ${IMU_SOURCES})

# サーボ用のソースファイルを追加
target_link_libraries(${PROJECT_NAME} pico_stdlib hardware_pwm)
include_directories(${CMAKE_SOURCE_DIR}/components/servo/sg90)
file(GLOB SERVO_SOURCES ${CMAKE_SOURCE_DIR}/components/servo/sg90/*.cpp)
target_sources(${PROJECT_NAME} PRIVATE ${SERVO_SOURCES})

# UDP通信用のソースファイルを追加
# .envファイルを読み込む
file(READ "${CMAKE_SOURCE_DIR}/.env" ENV_CONTENTS)
string(REGEX MATCH "WIFI_SSID=([^\n\r]*)" _ ${ENV_CONTENTS})
set(WIFI_SSID "${CMAKE_MATCH_1}")
string(REGEX MATCH "WIFI_PASSWORD=([^\n\r]*)" _ ${ENV_CONTENTS})
set(WIFI_PASSWORD "${CMAKE_MATCH_1}")
string(REGEX MATCH "UDP_TARGET=([^\n\r]*)" _ ${ENV_CONTENTS})
set(UDP_TARGET "${CMAKE_MATCH_1}")
string(REGEX MATCH "UDP_PORT=([^\n\r]*)" _ ${ENV_CONTENTS})
set(UDP_PORT "${CMAKE_MATCH_1}")
target_compile_definitions(DualShock4_on_picow PRIVATE
    WIFI_SSID=${WIFI_SSID}
    WIFI_PASSWORD=${WIFI_PASSWORD}
    UDP_TARGET=${UDP_TARGET}
    UDP_PORT=${UDP_PORT}
)
add_subdirectory(components/networking/UDP)
target_link_libraries(DualShock4_on_picow
    lib_picow_udp
)
target_include_directories(DualShock4_on_picow PRIVATE
    ${CMAKE_SOURCE_DIR}/components/networking/UDP
)

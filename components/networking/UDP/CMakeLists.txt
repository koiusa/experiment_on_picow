add_library(lib_picow_udp STATIC
    picow_udp.cpp
)

target_include_directories(lib_picow_udp PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${PICO_SDK_PATH}/lib/lwip/src/include # lwIPのインクルードパスを追加
    ${PICO_SDK_PATH}/lib/lwip/src/include/arch # アーキテクチャ固有のパスを追加
)

target_link_libraries(lib_picow_udp
    pico_cyw43_arch_lwip_threadsafe_background
    pico_stdlib
)

# OSC用のソースファイルを追加
add_subdirectory(OSC)
target_include_directories(lib_picow_udp PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/OSC/src
)
target_link_libraries(lib_picow_udp
    lib_osc
)
include_directories(${CMAKE_SOURCE_DIR}/OSC/src)
file(GLOB OSC_SOURCES ${CMAKE_SOURCE_DIR}/OSC/src/*.cpp)
target_sources(${PROJECT_NAME} PRIVATE ${OSC_SOURCES})
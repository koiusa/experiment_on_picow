add_library(lib_picow_udp STATIC
    picow_udp.cpp
)

target_include_directories(lib_picow_udp PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)
target_include_directories(lib_picow_udp PRIVATE
    ${PICO_SDK_PATH}/lib/lwip/src/include # lwIPのインクルードパスを追加
    ${PICO_SDK_PATH}/lib/lwip/src/include/arch # アーキテクチャ固有のパスを追加
)


target_link_libraries(lib_picow_udp
    pico_cyw43_arch_lwip_threadsafe_background
    pico_stdlib
    lib_osc
)

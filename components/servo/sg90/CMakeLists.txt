add_library(lib_sg90 STATIC
    sg90.cpp
)

target_link_libraries(lib_sg90
    pico_stdlib
    hardware_pwm
    lib_logic
)

target_include_directories(lib_sg90 PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

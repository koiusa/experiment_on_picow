add_library(lib_filter STATIC
    kalman.cpp
    madgwickAHRS.cpp
)

target_include_directories(lib_filter PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)
target_include_directories(lib_filter PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../math/matrix
)

target_link_libraries(lib_filter
    pico_stdlib
    lib_matrix
)
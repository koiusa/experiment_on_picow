add_library(lib_matrix STATIC
    eulerAngle.cpp
    quatanion.cpp
    rotationMatrix.cpp
)

target_include_directories(lib_matrix PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

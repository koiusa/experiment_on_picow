add_library(lib_sg90 STATIC
    sg90.cpp
)

target_include_directories(lib_sg90 PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

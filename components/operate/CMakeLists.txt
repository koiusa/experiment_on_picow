add_library(lib_operate STATIC
    debounce.cpp
)

target_include_directories(lib_operate PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

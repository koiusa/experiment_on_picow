add_library(lib_logic STATIC
    logic.cpp
)

target_include_directories(lib_logic PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

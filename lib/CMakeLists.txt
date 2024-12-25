add_library(lib_ds4_on_pico STATIC
	src/ds4_on_pico_w.cpp
)

target_link_libraries(lib_ds4_on_pico
    pico_stdlib
)

target_include_directories(lib_ds4_on_pico PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(lib_ds4_on_pico
	hardware_gpio
	pico_cyw43_arch_none
		pico_btstack_classic
		pico_btstack_ble
		pico_btstack_cyw43
	pico_multicore
)

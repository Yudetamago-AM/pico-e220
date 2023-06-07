add_library(pico_e220 INTERFACE)

target_sources(pico_e220 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/pico-e220.c
)

target_link_libraries(pico_e220
    INTERFACE
    pico_stdlib
    hardware_uart
)
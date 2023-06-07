add_subdirectory("motor_encoder")
add_executable(parent parent.c)

target_link_libraries(parent pico_stdlib hardware_uart pico_e220)

pico_enable_stdio_usb(parent 1)
pico_enable_stdio_uart(parent 0)

pico_add_extra_outputs(parent)

add_executable(child child.c)
target_link_libraries(child pico_stdlib hardware_uart motor_encoder pico_e220)

pico_enable_stdio_usb(child 1)
pico_enable_stdio_uart(child 0)

pico_add_extra_outputs(child)
cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)
include(../lib/RF24/CMakeLists.txt)
include(../lib/RF24Network/CMakeLists.txt)
include(../lib/RF24Mesh/CMakeLists.txt)


project(pico_huerta_project C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)


pico_sdk_init()

add_executable(pico_huerta
        main.cpp
)

pico_enable_stdio_usb(pico_huerta 0)
pico_enable_stdio_uart(pico_huerta 1)

pico_add_extra_outputs(pico_huerta)

# pull in common dependencies
target_link_libraries(pico_huerta pico_stdlib)

# add url via pico_set_program_url
#example_auto_set_url(blink)

cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)
include(./lib/RF24/CMakeLists.txt)
#include(./lib/RF24Network/CMakeLists.txt)
#include(./lib/RF24Mesh/CMakeLists.txt)


project(pico_huerta_project C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_BOARD pico_w)
set(PICO_BINARY_BRAIN power_brain)
set(PICO_BINARY_SCOUT plant_scout)

if(NOT WIFI_SSID OR NOT WIFI_PASSWORD)
        message(FATAL_ERROR "Wifi ssid or password not found, need to be condigured")
endif()

pico_sdk_init()

set(SOURCE_FILES      
        radio.cpp
        log.cpp
        wifi.cpp
        http.cpp
        board.cpp
)

set(BRAIN_SOURCES ${SOURCE_FILES} power_brain.cpp)
set(SCOUT_SOURCES ${SOURCE_FILES} plant_scout.cpp)

add_executable(${PICO_BINARY_BRAIN} ${BRAIN_SOURCES})
add_executable(${PICO_BINARY_SCOUT} ${SCOUT_SOURCES})

set(projects ${PICO_BINARY_BRAIN} ${PICO_BINARY_SCOUT})

foreach(binary IN LISTS projects)
        message(STATUS "Configuring ${binary}")        

        target_include_directories(${binary} PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
        target_link_libraries(${binary}
                pico_stdlib
                pico_cyw43_arch_lwip_threadsafe_background
                pico_lwip_http                          
                hardware_spi
                hardware_adc
                RF24
                
        )
        
        target_compile_definitions(${binary} PRIVATE
                WIFI_SSID=\"${WIFI_SSID}\"
                WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
                ARDUINO_RASPBERRY_PI_PICO_W
        )

        pico_enable_stdio_usb(${binary} 1)
        pico_enable_stdio_uart(${binary} 1)
        pico_add_extra_outputs(${binary})

        target_compile_definitions(${binary} PRIVATE
                #LWIP_HTTPD_CGI
                #LWIP_HTTPD_CGI_SSI
                LWIP_HTTPD_CUSTOM_FILES
                LWIP_DEBUG
                LWIP_DBG_TYPES_ON
                HTTPD_DEBUG
                HTTPD_DEBUG_TIMING
        )

endforeach()
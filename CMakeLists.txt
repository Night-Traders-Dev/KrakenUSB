cmake_minimum_required(VERSION 3.17)

# Set platform and board
set(PICO_PLATFORM rp2350-arm-s)
set(PICO_BOARD pico2)

include(${CMAKE_CURRENT_SOURCE_DIR}/pico_sdk_import.cmake)

project(KrakenMachine C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 26)
set(PICO_CXX_ENABLE_EXCEPTIONS 1)
set(FREERTOS_KERNEL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/freertos/)

# Initialize Pico SDK
pico_sdk_init()

# Add FreeRTOSConfig header
add_library(freertos_config INTERFACE)
target_include_directories(freertos_config INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/freertos/include
)

# Include TinyUSB, FreeRTOS
include(/opt/pico-sdk/lib/tinyusb/hw/bsp/family_support.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS_Kernel_import.cmake)

# Espressif-specific build system (skip if target is Espressif)
if(FAMILY STREQUAL "espressif")
  return()
endif()

# Add KrakenMachineScheduler library
add_library(KrakenMachineScheduler
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scheduler/kraken_scheduler.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scheduler/kraken_hal.c
)

# Include directories for KrakenMachineScheduler
target_include_directories(KrakenMachineScheduler PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scheduler
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usb
    ${CMAKE_CURRENT_SOURCE_DIR}/freertos/include
)

# Link libraries for KrakenMachineScheduler
target_link_libraries(KrakenMachineScheduler PRIVATE
    pico_stdlib
    pico_multicore
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Heap4
    freertos_config
)

# Define the main executable target
add_executable(KrakenMachine
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usb/storage.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usb/msc_disk.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usb/usb_descriptors.c
)

# Include directories for KrakenMachine
target_include_directories(KrakenMachine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scheduler
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usb
    ${CMAKE_CURRENT_SOURCE_DIR}/freertos/include
)

# Link libraries to the KrakenMachine executable
target_link_libraries(KrakenMachine PRIVATE
    pico_stdlib
    KrakenMachineScheduler
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Heap4
    freertos_config
    tinyusb_device
    tinyusb_board
    hardware_gpio
    hardware_flash
    hardware_uart
    hardware_spi
)

# Configure compilation flags and libraries for the target
family_configure_device_example(KrakenMachine noos)

# Enable STDIO via USB and UART
pico_enable_stdio_usb(KrakenMachine 1)
pico_enable_stdio_uart(KrakenMachine 1)

# Add extra outputs (e.g., UF2 binary)
pico_add_extra_outputs(KrakenMachine)

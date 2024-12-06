cmake_minimum_required(VERSION 3.17)

# Set platform and board
set(PICO_PLATFORM rp2350-arm-s)
set(PICO_BOARD pico2)

# Include TinyUSB family support
include(${CMAKE_CURRENT_SOURCE_DIR}/tinyusb/hw/bsp/family_support.cmake)

# Include FreeRTOS and Pico SDK
include(${CMAKE_CURRENT_SOURCE_DIR}/src/include/FreeRTOS_Kernel_import.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/src/include/pico_sdk_import.cmake)

project(KrakenMachine C CXX ASM)

# Initialize Pico SDK
pico_sdk_init()

# Espressif-specific build system (skip if target is Espressif)
if(FAMILY STREQUAL "espressif")
  return()
endif()

# Add KrakenMachineScheduler library
add_library(KrakenMachineScheduler
    src/scheduler/kraken_scheduler.c
    src/scheduler/kraken_hal.c
)

# Include directories for KrakenMachineScheduler
target_include_directories(KrakenMachineScheduler PRIVATE
    freertos
    freertos/include
    src
    src/include
    src/scheduler
    src/usb
)

# Link libraries for KrakenMachineScheduler
target_link_libraries(KrakenMachineScheduler
    pico_stdlib
    pico_multicore
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Heap4
)

# Define the main executable target
add_executable(KrakenMachine
    src/main.c
    src/usb/storage.c
    src/usb/msc_disk.c
    src/usb/usb_descriptors.c
)

# Include directories for KrakenMachine
target_include_directories(KrakenMachine PRIVATE
    src
    src/include
    src/scheduler
    src/usb
)

# Link libraries to the KrakenMachine executable
target_link_libraries(KrakenMachine PRIVATE
    pico_stdlib
    KrakenMachineScheduler
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Heap4
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

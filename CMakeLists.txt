cmake_minimum_required(VERSION 3.17)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Include TinyUSB family support
include(${CMAKE_CURRENT_SOURCE_DIR}/tinyusb/hw/bsp/family_support.cmake)

# Include FreeRTOS Kernel
include(src/include/FreeRTOS_Kernel_import.cmake)

# Get project name for the example (e.g., <BOARD>-<DIR_NAME>)
family_get_project_name(PROJECT ${CMAKE_CURRENT_SOURCE_DIR})

# Define the project
project(${PROJECT} C CXX ASM)

# Initialize the project for the target family
family_initialize_project(${PROJECT} ${CMAKE_CURRENT_SOURCE_DIR})

# Espressif has its own build system, skip configuration if target is Espressif
if(FAMILY STREQUAL "espressif")
  return()
endif()

# Add KrakenMachineScheduler library
add_library(KrakenMachineScheduler
    src/scheduler/kraken_scheduler.c
    src/scheduler/kraken_hal.c
)

target_include_directories(KrakenMachineScheduler PRIVATE
#    freertos
#    freertos/include
    src
    src/include
    src/scheduler
    src/usb
)
target_link_libraries(KrakenMachineScheduler
    pico_stdlib
    pico_multicore
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Heap4
)

# Define the executable target
add_executable(${PROJECT})

# Add source files for the project
target_sources(${PROJECT} PRIVATE
    src/main.c
    src/scheduler/kraken_scheduler.c
    src/scheduler/kraken_hal.c
    src/usb/storage.c
    src/usb/msc_disk.c
    src/usb/usb_descriptors.c
)

# Include directories for the project
target_include_directories(${PROJECT} PRIVATE
#    freertos
#    freertos/include
    src
    src/include
    src/scheduler
    src/usb
)

# Link libraries to the main executable
target_link_libraries(${PROJECT} PRIVATE
    pico_stdlib
    pico_multicore
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
family_configure_device_example(${PROJECT} noos)

# Add extra outputs (e.g., UF2 binary)
pico_add_extra_outputs(${PROJECT})

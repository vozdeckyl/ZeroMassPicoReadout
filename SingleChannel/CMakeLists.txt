cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(ZeroMassPicoSingleChannel)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()


add_executable(singleChannel
    singleChannel.c
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(uart pico_stdlib pico_multicore hardware_adc)

# enable usb output, disable uart output
#pico_enable_stdio_uart(uart 1)
pico_enable_stdio_usb(uart 1)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(uart)

cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(ZeroMassPicoSingleChannelBinary)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()


add_executable(singleChannel
    singleChannelBinary.c
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(singleChannel pico_stdlib pico_multicore hardware_adc)

# enable/disable usb/uart I/O
pico_enable_stdio_uart(singleChannel 1)
#pico_enable_stdio_usb(singleChannel 1)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(singleChannel)

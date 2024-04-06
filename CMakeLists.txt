cmake_minimum_required(VERSION 3.22)
project(NextEmu)

add_subdirectory(builtin)
#add_subdirectory(modules)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/builtin/platform/rvcore_spike/include)
include_directories(${PROJECT_SOURCE_DIR}/builtin/device/uart_rtl/include)
include_directories(${PROJECT_SOURCE_DIR}/builtin/device/uartlite/include)
#include_directories(${PROJECT_SOURCE_DIR}/builtin/device/bus/include)
include_directories(${PROJECT_SOURCE_DIR}/builtin/device/mem/include)
include_directories(${PROJECT_SOURCE_DIR}/builtin/waveform_encoder/uart_encoder/include)

set(CMAKE_CXX_STANDARD 17)

file(GLOB HPP_FILES "${PROJECT_SOURCE_DIR}/include/sdk/base/*.hpp")

add_executable(NextEmu main.cpp test.cpp ${HPP_FILES})

target_link_libraries(NextEmu
        builtin_mem
        builtin_addr_bus
        builtin_rvcore_spike
        builtin_rvcore
        uart_rtl
        builtin_uart_encoder
        builtin_uartlite)

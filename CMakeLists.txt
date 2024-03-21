cmake_minimum_required(VERSION 3.25)
project(NextEmu)

add_subdirectory(builtin)
#add_subdirectory(modules)

include_directories(${PROJECT_SOURCE_DIR}/include)

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

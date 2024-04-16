cmake_minimum_required(VERSION 3.22)
project(NextEmu)

add_subdirectory(builtin)
#add_subdirectory(modules)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/builtin/platform/rvcore_spike/include)
include_directories(${PROJECT_SOURCE_DIR}/builtin/device/uart_rtl/include)
include_directories(${PROJECT_SOURCE_DIR}/builtin/device/uartlite_rtl/include)
include_directories(${PROJECT_SOURCE_DIR}/builtin/device/uartlite/include)
include_directories(${PROJECT_SOURCE_DIR}/builtin/device/dummy/include)
#include_directories(${PROJECT_SOURCE_DIR}/builtin/device/bus/include)
include_directories(${PROJECT_SOURCE_DIR}/builtin/device/mem/include)
include_directories(${PROJECT_SOURCE_DIR}/builtin/rtl_adapter/uart_encoder/include)
include_directories(${PROJECT_SOURCE_DIR}/builtin/rtl_adapter/axilite/include)

set(CMAKE_CXX_STANDARD 17)

file(GLOB HPP_FILES "${PROJECT_SOURCE_DIR}/include/sdk/base/*.hpp")

aux_source_directory(${PROJECT_SOURCE_DIR}/src MODULE_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/test MODULE_TEST_SRC)

add_executable(NextEmu main.cpp test.cpp ${HPP_FILES} ${MODULE_SRC} ${MODULE_TEST_SRC})

target_link_libraries(NextEmu
        builtin_mem
        builtin_addr_bus
        builtin_rvcore_spike
        builtin_rvcore
        uart_rtl
        uartlite_rtl
        builtin_uart_encoder
        builtin_uartlite
        builtin_dummydev
        builtin_axi4lite)

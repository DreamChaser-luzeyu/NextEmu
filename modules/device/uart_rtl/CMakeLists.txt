message("Built-in uart_rtl at ${PROJECT_SOURCE_DIR}")

include_directories(include)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rtl/include/)
include_directories(${PROJECT_SOURCE_DIR}/include)

include(${PROJECT_SOURCE_DIR}/cmake/global_cfg.cmake)
find_package(verilator HINTS ${VERILATOR_ROOT})
if (NOT verilator_FOUND)
    message(FATAL_ERROR "Verilator was not found. Either install it, or set the VERILATOR_ROOT environment variable")
endif()

aux_source_directory(src MODULE_SRC)

set(SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
        ${MODULE_SRC}
)

add_library(uart_rtl ${SOURCE_FILES})
verilate(uart_rtl SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/rtl/UartRx.v")


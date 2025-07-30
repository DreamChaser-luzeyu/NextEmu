message("Built-in axi4lite at ${CMAKE_CURRENT_SOURCE_DIR}")

include(${PROJECT_SOURCE_DIR}/cmake/verilator.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/private_include)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rtl/include)
include_directories(${PROJECT_SOURCE_DIR}/include)

include(${PROJECT_SOURCE_DIR}/cmake/global_cfg.cmake)
find_package(verilator HINTS ${VERILATOR_ROOT})
if (NOT verilator_FOUND)
    message(FATAL_ERROR "Verilator was not found. Either install it, or set the VERILATOR_ROOT environment variable")
endif()

# ----- C++ srcs
aux_source_directory(src MODULE_SRC)
set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp ${MODULE_SRC})
# ----- HDL srcs
file(GLOB RTL_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/rtl/Top.v")

add_library(builtin_axi4lite ${SOURCE_FILES})
verilate(builtin_axi4lite SOURCES "${RTL_SRC_FILES}")

message("Built-in axi-encoder at ${PROJECT_SOURCE_DIR}")

include_directories(include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rtl/include/)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(/usr/share/verilator/include)

# ----- Clean Generated Files
execute_process(COMMAND rm -rf include WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/rtl")
execute_process(COMMAND rm -rf src WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/rtl")
execute_process(COMMAND rm -rf obj_dir WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/rtl")

# ----- RTL Sources
file(GLOB RTL_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/rtl/*.v")
file(GLOB RTL_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/rtl/*.sv")
message("RTL Source List:")
foreach (RTL_SRC_FILE ${RTL_SRC_FILES})
    message("    ${RTL_SRC_FILE}")
endforeach ()
# ----- Gen Top Module
#execute_process(
#        COMMAND verilator
##        -v
##
#        --cc
#        --Wno-WIDTH
#        --Wno-UNOPTFLAT
#        --Wno-CASEINCOMPLETE
#        --Wno-MULTIDRIVEN
#        --Wno-SELRANGE
#        --Wno-SYMRSVDWORD
#        --trace
#        "axi_pkg.sv"
#        "axi_from_mem.sv"
#        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/rtl"
#)

# ----- mkdir
execute_process(COMMAND mkdir include WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/rtl")
execute_process(COMMAND mkdir src WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/rtl")

# ----- Verilator Generated Files
# --- Verilator Generated Sources
file(GLOB VG_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/rtl/obj_dir/*.cpp")
foreach (VG_SRC_FILE ${VG_SRC_FILES})
    execute_process(
            COMMAND cp ${VG_SRC_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/rtl/src/
    )
endforeach ()
# --- Verilator Generated Headers
file(GLOB VG_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/rtl/obj_dir/*.h")
foreach (VG_SRC_FILE ${VG_SRC_FILES})
    execute_process(
            COMMAND cp ${VG_SRC_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/rtl/include/
    )
endforeach ()

# ----- Specify location of `verilated.cpp` & `verilated_vcd_c.cpp`
set(VERILATOR_SRCS /usr/share/verilator/include/verilated.cpp /usr/share/verilator/include/verilated_vcd_c.cpp)

# ----- C++ Sources
aux_source_directory(src MODULE_SRC)
aux_source_directory(rtl/src RTL_SRC)


set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp ${MODULE_SRC} ${RTL_SRC} ${VERILATOR_SRCS})

add_library(axi_encoder ${SOURCE_FILES})

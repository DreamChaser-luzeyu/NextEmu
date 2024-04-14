message("Built-in uartlite_rtl at ${PROJECT_SOURCE_DIR}")

include_directories(include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rtl/include/)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(/usr/local/share/verilator/include)

# ----- Clean Generated Files
execute_process(COMMAND rm -rf include WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/rtl")
execute_process(COMMAND rm -rf src WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/rtl")
execute_process(COMMAND rm -rf obj_dir WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/rtl")

# ----- RTL Sources
file(GLOB RTL_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/rtl/*.v")
message("RTL Source List:")
foreach (RTL_SRC_FILE ${RTL_SRC_FILES})
    message("    ${RTL_SRC_FILE}")
    execute_process(
            COMMAND verilator --cc --trace ${RTL_SRC_FILE}
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/rtl"
    )
endforeach ()

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
# --- Generate `.a` libs
file(GLOB VG_MK_FILES "${CMAKE_CURRENT_SOURCE_DIR}/rtl/obj_dir/*.mk")
foreach (VG_MK_FILE ${VG_MK_FILES})
    execute_process(
            COMMAND make -f ${VG_MK_FILE}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/rtl/obj_dir/
    )
endforeach ()
execute_process(
        COMMAND mkdir libs
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/rtl/obj_dir/
)
# --- Move `.a` libs to libs/
file(GLOB VG_AR_FILES "${CMAKE_CURRENT_SOURCE_DIR}/rtl/obj_dir/*.a")
foreach (VG_AR_FILE ${VG_AR_FILES})
    execute_process(
            COMMAND mv ${VG_AR_FILE} libs/
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/rtl/obj_dir/
    )
endforeach ()
# --- Add those libs
file(GLOB VG_LIB_FILES "${CMAKE_CURRENT_SOURCE_DIR}/rtl/obj_dir/libs/lib*")
foreach (VG_LIB_FILE ${VG_LIB_FILES})
    message("Add lib ${VG_LIB_FILE}")
    get_filename_component(LIB_NAME ${VG_LIB_FILE} NAME_WE)
    add_library(${LIB_NAME} STATIC IMPORTED)
    set_target_properties(${LIB_NAME} PROPERTIES IMPORTED_LOCATION ${VG_LIB_FILE})
endforeach ()

# ----- C++ Sources
aux_source_directory(src MODULE_SRC)
#aux_source_directory(rtl/src RTL_SRC)
#aux_source_directory(/usr/local/share/verilator/include VERILATOR_SRCS)

set(SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
        ${MODULE_SRC}
)

add_library(uartlite_rtl ${SOURCE_FILES})
target_link_libraries(uartlite_rtl PRIVATE ${VG_LIB_FILES})


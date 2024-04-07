message("Built-in axi4lite at ${CMAKE_CURRENT_SOURCE_DIR}")

include(${PROJECT_SOURCE_DIR}/cmake/verilator.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/rtl/include)
include_directories(${PROJECT_SOURCE_DIR}/include)

# ----- C++ srcs
aux_source_directory(src MODULE_SRC)
set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp)
# ----- HDL srcs
file(GLOB RTL_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/rtl/AXIMaster.v")
# ----- Verilator Build
VerilatorBuild(${RTL_SRC_FILES})
# ----- Add verilator generated libs
file(GLOB VG_LIB_FILES "${CMAKE_CURRENT_SOURCE_DIR}/rtl/obj_dir/libs/lib*")
foreach (VG_LIB_FILE ${VG_LIB_FILES})
    message("Add lib ${VG_LIB_FILE}")
    get_filename_component(LIB_NAME ${VG_LIB_FILE} NAME_WE)
    add_library(${LIB_NAME} STATIC IMPORTED)
    set_target_properties(${LIB_NAME} PROPERTIES IMPORTED_LOCATION ${VG_LIB_FILE})
endforeach ()

add_library(builtin_axi4lite ${SOURCE_FILES})
target_link_libraries(builtin_axi4lite PRIVATE ${VG_LIB_FILES})

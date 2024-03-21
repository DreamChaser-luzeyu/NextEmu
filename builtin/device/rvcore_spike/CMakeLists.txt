message("Built-in mem at ${PROJECT_SOURCE_DIR}")


set(SPIKE_REPO_ROOT_DIR ${PROJECT_SOURCE_DIR}/misc/riscv-isa-sim)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/misc/include)
include_directories(${SPIKE_REPO_ROOT_DIR}/fdt)
include_directories(${SPIKE_REPO_ROOT_DIR}/fesvr)
include_directories(${SPIKE_REPO_ROOT_DIR}/softfloat)
include_directories(${SPIKE_REPO_ROOT_DIR}/build)
include_directories(${SPIKE_REPO_ROOT_DIR})
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${SPIKE_REPO_ROOT_DIR}/riscv)

aux_source_directory(src MODULE_SRC)
#aux_source_directory(misc/riscv-isa-sim/riscv MODULE_SRC)

set(SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
        ${MODULE_SRC}
        )

add_library(spike_libdisasm STATIC IMPORTED)
set_target_properties(spike_libdisasm PROPERTIES IMPORTED_LOCATION ${SPIKE_REPO_ROOT_DIR}/build/libdisasm.a)

add_library(spike_libfdt STATIC IMPORTED)
set_target_properties(spike_libfdt PROPERTIES IMPORTED_LOCATION ${SPIKE_REPO_ROOT_DIR}/build/libfdt.a)

add_library(spike_libfesvr STATIC IMPORTED)
set_target_properties(spike_libfesvr PROPERTIES IMPORTED_LOCATION ${SPIKE_REPO_ROOT_DIR}/build/libfesvr.a)

add_library(spike_libriscv STATIC IMPORTED)
set_target_properties(spike_libriscv PROPERTIES IMPORTED_LOCATION ${SPIKE_REPO_ROOT_DIR}/build/libriscv.a)

add_library(spike_libsoftfloat STATIC IMPORTED)
set_target_properties(spike_libsoftfloat PROPERTIES IMPORTED_LOCATION ${SPIKE_REPO_ROOT_DIR}/build/libsoftfloat.a)

add_library(spike_libspike_dasm STATIC IMPORTED)
set_target_properties(spike_libspike_dasm PROPERTIES IMPORTED_LOCATION ${SPIKE_REPO_ROOT_DIR}/build/libspike_dasm.a)

add_library(spike_libspike_main STATIC IMPORTED)
set_target_properties(spike_libspike_main PROPERTIES IMPORTED_LOCATION ${SPIKE_REPO_ROOT_DIR}/build/libspike_main.a)


add_library(builtin_rvcore_spike ${SOURCE_FILES})
# Have to be in this sequence!
target_link_libraries(builtin_rvcore_spike PRIVATE
#        spike_libspike_main
        spike_libriscv
        spike_libdisasm
        spike_libsoftfloat
        spike_libfesvr
        spike_libfdt
        pthread
        boost_regex
        boost_system)



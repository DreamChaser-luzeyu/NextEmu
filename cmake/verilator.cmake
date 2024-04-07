include(${CMAKE_CURRENT_LIST_DIR}/global_cfg.cmake)

include_directories(${VERILATOR_PATH}/include)

function(VerilatorBuild rtl_file_list)
    # ----- Clean Generated Files
    execute_process(COMMAND rm -rf include WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/rtl")
    execute_process(COMMAND rm -rf obj_dir WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/rtl")
    # ----- Generate classes
    foreach (RTL_SRC_FILE ${rtl_file_list})
        message("    ${RTL_SRC_FILE}")
        execute_process(
                COMMAND verilator --cc --trace ${RTL_SRC_FILE}
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/rtl"
        )
    endforeach ()
    # ----- Copy headers
    execute_process(COMMAND mkdir include WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/rtl")
    file(GLOB VG_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/rtl/obj_dir/*.h")
    foreach (VG_SRC_FILE ${VG_SRC_FILES})
        execute_process(
                COMMAND cp ${VG_SRC_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/rtl/include/
        )
    endforeach ()
    # ----- Build libs
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
    # ----- Copy libs
    file(GLOB VG_AR_FILES "${CMAKE_CURRENT_SOURCE_DIR}/rtl/obj_dir/*.a")
    foreach (VG_AR_FILE ${VG_AR_FILES})
        execute_process(
                COMMAND mv ${VG_AR_FILE} libs/
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/rtl/obj_dir/
        )
    endforeach ()
endfunction()

function(AddLibs libs_folder)
    file(GLOB VG_LIB_FILES "${libs_folder}/lib*")
    foreach (VG_LIB_FILE ${VG_LIB_FILES})
        message("Add lib ${VG_LIB_FILE}")
        get_filename_component(LIB_NAME ${VG_LIB_FILE} NAME_WE)
        add_library(${LIB_NAME} STATIC IMPORTED)
        set_target_properties(${LIB_NAME} PROPERTIES IMPORTED_LOCATION ${VG_LIB_FILE})
    endforeach ()
endfunction()
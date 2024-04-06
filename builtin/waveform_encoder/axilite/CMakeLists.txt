message("Built-in axi4lite at ${CMAKE_CURRENT_SOURCE_DIR}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include)

aux_source_directory(src MODULE_SRC)

set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp)

add_library(builtin_axi4lite ${SOURCE_FILES})

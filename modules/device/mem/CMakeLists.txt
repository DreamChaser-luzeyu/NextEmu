message("Built-in mem at ${PROJECT_SOURCE_DIR}")

include_directories(include)
include_directories(${PROJECT_SOURCE_DIR}/include)

aux_source_directory(src MODULE_SRC)

set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp ${MODULE_SRC})

add_library(builtin_mem ${SOURCE_FILES})

project(cad)

cmake_minimum_required(VERSION 3.17)

include_directories("include")
include_directories("ext/inc")
file(GLOB_RECURSE SOURCES "source/*.cpp")

#add optimization flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")


add_executable(exe ${SOURCES} "demo_graphics.cpp")
target_link_libraries(exe "${CMAKE_SOURCE_DIR}/ext/bin/libglfw3.a")
target_link_libraries(exe "${CMAKE_SOURCE_DIR}/ext/bin/libglad.a")
target_link_libraries(exe m X11 stdc++ GL)

set_target_properties(exe PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)
project(DidactiCAD)

cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

include_directories("Include")

file(GLOB_RECURSE SOURCES "Source/*.cpp")
add_executable(DidactiCAD ${SOURCES})

find_package(OpenGL REQUIRED)
target_link_libraries(DidactiCAD OpenGL::GL)

find_package(glfw3 REQUIRED)
target_link_libraries(DidactiCAD glfw)

find_package(GLEW REQUIRED)
target_link_libraries(DidactiCAD GLEW::GLEW)

target_link_libraries(DidactiCAD stdc++)
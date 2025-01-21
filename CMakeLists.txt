cmake_minimum_required(VERSION 3.10.0)
project(hello_window VERSION 1.0.0)

find_package(OpenGL REQUIRED)

add_executable(hello_window src/main.cpp)

target_compile_definitions(hello_window PRIVATE GLEW_STATIC)

target_include_directories(hello_window PRIVATE include include/imgui)

target_link_directories(hello_window PRIVATE lib/GLFW lib/GL)

target_link_libraries(hello_window PRIVATE libglfw3.a glew32s.lib OpenGL::GL)
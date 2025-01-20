cmake_minimum_required(VERSION 3.10.0)
project(hello_window VERSION 1.0.0)

find_package(OpenGL REQUIRED)

add_executable(hello_window src/config.h src/main.cpp src/glad.c)

target_include_directories(hello_window PRIVATE include)

target_link_directories(hello_window PRIVATE lib/GLFW)

target_link_libraries(hello_window PRIVATE libglfw3.a OpenGL::GL)
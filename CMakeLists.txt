cmake_minimum_required(VERSION 3.10.0)
include(FetchContent)
project(FractalDive VERSION 1.0.0)

find_package(OpenGL REQUIRED)

find_package(glfw3 3.4 QUIET)
if(NOT glfw3_FOUND)
	FetchContent_Declare(
		glfw3
		GIT_REPOSITORY	https://github.com/glfw/glfw.git
		GIT_TAG			3.4
	)
	FetchContent_MakeAvailable(glfw3)
endif()

find_package(GLEW QUIET)
if(NOT GLEW_FOUND)

	set(GLEW_BUILD_DOCS OFF CACHE BOOL "Disable documentation generation")
	set(glew-cmake_BUILD_SHARED OFF CACHE BOOL "" FORCE)
	set(glew-cmake_BUILD_STATIC ON CACHE BOOL "" FORCE)

	FetchContent_Declare(
		glew
		GIT_REPOSITORY 	https://github.com/Perlmint/glew-cmake.git
		GIT_TAG			glew-cmake-2.2.0
	)
	FetchContent_MakeAvailable(glew)

	set(GLEW_LIB libglew_static)
else()
	set(GLEW_LIB GLEW::GLEW)
endif()

find_package(glm 1.0.1 QUIET)
if(NOT glm_FOUND)
	FetchContent_Declare(
		glm
		GIT_REPOSITORY	https://github.com/g-truc/glm.git
		GIT_TAG			1.0.1
	)
	FetchContent_MakeAvailable(glm)
endif()

file(GLOB IMGUI_SOURCES 
	"include/imgui/*.cpp" 
	"include/imgui/backends/imgui_impl_glfw.cpp" 
	"include/imgui/backends/imgui_impl_opengl3.cpp")

file(GLOB SOURCES "*.cpp" "src/*.cpp")

add_executable(FractalDive ${SOURCES} ${IMGUI_SOURCES})

target_compile_definitions(FractalDive PRIVATE GLEW_STATIC)

target_include_directories(FractalDive PRIVATE include include/imgui)

target_link_libraries(FractalDive PRIVATE glfw ${GLEW_LIB} glm::glm OpenGL::GL)
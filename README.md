# FractalDive

A small project built using C++ and OpenGL that enables the user to explore both the Mandelbrot and Julia sets using GLSL shaders for efficient fractal rendering. Any dependencies apart from OpenGL and ImGui are automatically fetched from source and built together with the project. For details, see [Acknowledgements](#acknowledgements)

**Note**: The project has been built and tested on **Windows 10** and **Ubuntu 24.04.2 LTS**. Windows 11 has not been tested but should work out of the box, and any further mention of Linux will refer specifically to Ubuntu 24.04.2 LTS. MacOS has not been verified as I don't have access to such a machine.

![Fractal Visualization Demo](demo/demo.gif)

## Prerequisites

### Windows & Linux
- **C++ Compiler** - The project was built and tested with MinGW on both Windows and Linux, but others should work.
- **CMake** - Used to automate the generation of project files.
- **OpenGL** - OpenGL and appropriate drivers need to be installed.

### Linux Extras

In addition to the basic requirements, on Linux you might have to install additional packages in order to build the program. I used Ubuntu when testing and had to run the following commands:
```bash
sudo apt update && sudo apt upgrade

# Install the Mesa OpenGL development libraries, ensures proper OpenGL headers and libraries
sudo apt install libgl1-mesa-dev

# Install X11 development libraries for GLFW support
sudo apt install libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
```

## Building
1. Clone the repository:
```bash
git clone https://github.com/Yeenasour/FractalDive.git
cd FractalDive
```

2. Generate the project files using CMake
```bash
mkdir build
cd build
cmake ..
```

3. Building & Running the Project
- Windows - Open the generated solution in Visual Studio or with VSCode and build the project. After build has finished, run directly through the IDE or execute the binary located in the build folder.
- Linux
```bash
make
./FractalDive
```

## Usage
When running the project you have the following controls:
- **Mouse**
	- **Left Click & Drag**: Used for panning the viewplane
	- **Right Click & Drag**: Only used to modify the Julia-Set "c" constant, only works at low zoom levels
	- **Scroll Wheel / Trackpad**: Used for zooming
- **Keyboard**
	- **WASD**: Used for panning the viewplane

## Acknowledgements

This Project depends on the following libraries and frameworks to run:
- [**GLEW-cmake**](https://github.com/Perlmint/glew-cmake): A CMake-compatible fork of GLEW
- [**GLFW**](https://github.com/glfw/glfw): C++ library for handling window creation and managing inputs
- [**GLM**](https://github.com/g-truc/glm): OpenGL Mathematics, in this project is used minimally
- [**Dear ImGui**](https://github.com/ocornut/imgui): Library necessary for creation of GUI components, included with the project.

## License
This project is licensed under the [MIT License](LICENSE).

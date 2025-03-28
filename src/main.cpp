#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <FileUtils.h>
#include <Shader.h>

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

enum {
	MANDELBROT,
	JULIASET
};

struct WindowState
{
	float cx, cy;
	float zoom;
	int w, h;
};

struct ApplicationState
{
	WindowState window;
	std::unordered_map<int, bool> keyMap;
	double mouseClickX, mouseClickY;
	double prevCx, prevCy;
	double juliaCx = NAN, juliaCy = NAN;
	bool leftButtonHeld = false, rightButtonHeld = false;
};

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
	WindowState* ws = &(static_cast<ApplicationState*>(glfwGetWindowUserPointer(window)))->window;
	ws->w = width;
	ws->h = height;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	WindowState* ws = &(static_cast<ApplicationState*>(glfwGetWindowUserPointer(window)))->window;
	static double zoomInFactor = 1.1;
	constexpr static double zoomOutFactor = 1.0 / 1.1;

	if (yoffset > 0)
	{
		ws->zoom *= zoomInFactor;
	}
	else
	{
		if (ws->zoom <= 1) return;
		ws->zoom *= zoomOutFactor;
	}
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	ApplicationState* as = static_cast<ApplicationState*>(glfwGetWindowUserPointer(window));
	//std::unordered_map<int, bool> *keyMap = as->keyMap;
	auto& keyMap = as->keyMap;
	if (keyMap.find(key) != keyMap.end())
	{
		if (action == GLFW_PRESS)
		{
			keyMap[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keyMap[key] = false;
		}
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
	ApplicationState* as = static_cast<ApplicationState*>(glfwGetWindowUserPointer(window));
	double dx = as->mouseClickX - xpos;
    double dy = ypos - as->mouseClickY;

	// 135 is from 1080 / 8,
    double coordX = as->prevCx + dx / (135 * as->window.zoom); 
	double coordY = as->prevCy + dy / (135 * as->window.zoom);
	if (as->leftButtonHeld)
	{
        as->window.cx = coordX;
        as->window.cy = coordY;
	}
	else if (as->rightButtonHeld)
	{
		if (std::isnan(as->juliaCx) || as->window.zoom > 8) return;
        as->juliaCx = coordX;
        as->juliaCy = coordY;
	}
}

void mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (ImGui::GetIO().WantCaptureMouse) return;
	ApplicationState* as = static_cast<ApplicationState*>(glfwGetWindowUserPointer(window));
	as->leftButtonHeld = (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS);
	as->rightButtonHeld = (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS);
	glfwGetCursorPos(window, &as->mouseClickX, &as->mouseClickY);
	if (as->leftButtonHeld)
	{
		as->prevCx = as->window.cx;
    	as->prevCy = as->window.cy;
	}
}

void handleKeyMovement(ApplicationState &as, double deltaTime)
{
	double moveSpeed = 2.5f * deltaTime / as.window.zoom;
	float dy = 0.0f, dx = 0.0f;
	if (as.keyMap[GLFW_KEY_W]) dy += moveSpeed;
	if (as.keyMap[GLFW_KEY_A]) dx -= moveSpeed;
	if (as.keyMap[GLFW_KEY_S]) dy -= moveSpeed;
	if (as.keyMap[GLFW_KEY_D]) dx += moveSpeed;
	as.window.cx += dx;
	as.window.cy += dy;
}

ImVec4 GetButtonColor(bool isActive) {
    return isActive ? ImVec4(0.0, 0.4, 1.0, 0.5) : ImVec4(0.0, 0.0, 0.0, 0.5);
}

int main()
{
	GLFWwindow* window;

	int maxIterations = 128;
	int baseIterations = 128;
	float saturation = 1.0f;
	float brightness = 1.0f;
	
	ApplicationState applicationState = {
		{-0.5, 0, 2.0, 1080, 1080},
		{
			{GLFW_KEY_W, false},
			{GLFW_KEY_A, false},
			{GLFW_KEY_S, false},
			{GLFW_KEY_D, false}
		}
	};

	if (!glfwInit()) {
		std::cout << "GLFW failed to initialize" << std::endl;
		return -1;
	}

	//glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
	window = glfwCreateWindow(applicationState.window.w, applicationState.window.h, "FractalDive", NULL, NULL);
	glfwMakeContextCurrent(window);

	glfwSetWindowUserPointer(window, &applicationState);
	
	glfwSetCursorPosCallback(window, cursorPositionCallback);
	glfwSetMouseButtonCallback(window, mouseCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetScrollCallback(window, scrollCallback);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW failed to initialize" << std::endl;
		return -1;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;

	float position[] = {
		-1.0f, -1.0f,
		 1.0f,  1.0f,
		 1.0f, -1.0f,
		-1.0f,  1.0f,
	};

	GLuint vertecies;
	glGenBuffers(1, &vertecies);
	glBindBuffer(GL_ARRAY_BUFFER, vertecies);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), position, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	unsigned int indicies[] = {
		0, 2, 1,
		0, 1, 3
	};

	GLuint index;
	glGenBuffers(1, &index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indicies, GL_STATIC_DRAW);

	Shader program = Shader("../src/shaders/shader.vert", "../src/shaders/shader.frag");
	program.use();

	program.setUniform1f("u_zoom", applicationState.window.zoom);
	program.setUniform2i("u_resolution", applicationState.window.w, applicationState.window.h);
	program.setUniform2f("u_center", applicationState.window.cx, applicationState.window.cy);
	program.setUniform1i("u_MAX_ITERATIONS", maxIterations);
	program.setUniform1i("u_BASE_ITERATIONS", baseIterations);
	program.setUniform1f("u_saturation", saturation);
	program.setUniform1f("u_brightness", brightness);
	program.setUniform2f("u_julia_c", applicationState.juliaCx, applicationState.juliaCy);

	glEnable(GL_CULL_FACE);

	int maxFPS;

	{
		GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
		maxFPS = mode->refreshRate;
	}

	int targetFPS = maxFPS;
	float targetFrameTime = 1.0f / targetFPS;
	double lastDrawTime = glfwGetTime();
	double lastKeyTime = glfwGetTime();

	while (!glfwWindowShouldClose(window)) 
	{
		glfwPollEvents();

		double currentTime = glfwGetTime();
		double deltaDrawTime = currentTime - lastDrawTime;
		double deltaKeyTime = currentTime - lastKeyTime;
		handleKeyMovement(applicationState, deltaKeyTime);
		lastKeyTime = currentTime;

		if (deltaDrawTime > targetFrameTime) {
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ImGui::BeginGroup();
			ImGui::Text("Fractal Type");
			ImGui::PushStyleColor(ImGuiCol_Button, GetButtonColor(std::isnan(applicationState.juliaCx)));
			if (ImGui::Button("Mandelbrot"))
			{
				applicationState.juliaCx = NAN;
				applicationState.juliaCy = NAN;
				applicationState.window.zoom = 2.0,
				applicationState.window.cx = -0.5;
				applicationState.window.cy = 0.0;
			}
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Button, GetButtonColor(!std::isnan(applicationState.juliaCx)));
			if (ImGui::Button("Julia"))
			{
				applicationState.juliaCx = 0.0;
				applicationState.juliaCy = 0.0;
				applicationState.window.zoom = 2.0;
				applicationState.window.cx = 0.0;
				applicationState.window.cy = 0.0;
			}
			ImGui::PopStyleColor(2);
			ImGui::EndGroup();

			if (ImGui::SliderInt("FPS Limit", &targetFPS, 1, maxFPS)) {
				targetFrameTime = 1.0f / targetFPS;  // Update the target frame time
			}
			if (ImGui::SliderInt("U_MAX_ITERATIONS", &maxIterations, 1, 1024))
			{
				program.setUniform1i("u_MAX_ITERATIONS", maxIterations);
			}
			if (ImGui::SliderInt("U_BASE_ITERATIONS", &baseIterations, 1, 1024))
			{
				program.setUniform1i("u_BASE_ITERATIONS", baseIterations);
			}

			program.setUniform1f("u_zoom", applicationState.window.zoom);
			program.setUniform2i("u_resolution", applicationState.window.w, applicationState.window.h);
			program.setUniform2f("u_center", applicationState.window.cx, applicationState.window.cy);
			program.setUniform2f("u_julia_c", applicationState.juliaCx, applicationState.juliaCy);

			ImGui::BeginGroup();
			ImGui::Text("Color Controls");
			float sliderWidth = (ImGui::GetContentRegionAvail().x / 2.0f) - 10.0f;
			ImGui::PushItemWidth(sliderWidth);
			if(ImGui::SliderFloat("Saturation", &saturation, 0, 1))
			{
				program.setUniform1f("u_saturation", saturation);
			}
			if(ImGui::SliderFloat("Brightness", &brightness, 0, 1))
			{
				program.setUniform1f("u_brightness", brightness);
			}
			ImGui::PopItemWidth();
			ImGui::EndGroup();

			lastDrawTime = currentTime;
			glClear(GL_COLOR_BUFFER_BIT);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);


			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(window);
		}
	}
	
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
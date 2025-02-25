#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>


struct WindowState
{
	float cx, cy;
	float zoom;
	int w, h;
};

struct ApplicationState
{
	WindowState window;
	std::unordered_map<int, bool>* keyMap;
	double mouseEnterX, mouseEnterY;
	float dragStartCx, dragStartCy;
	bool mouseHeld = false;
};

static std::string readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

static GLuint CompileShader(GLuint type, const std::string& source)
{
	GLuint shader = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	GLint res;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &res);
	if (res == GL_FALSE)
	{
		int len;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
		char* msg = (char*)alloca(len * sizeof(char));
		glGetShaderInfoLog(shader, len, &len, msg);
		std::cout << "Shader compilation failed! (" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << ")" << std::endl;
		std::cout << msg << std::endl;
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

static GLuint CreateShaderProgram(const std::string& vertexShader, const std::string& fragmentShader)
{
	GLuint program = glCreateProgram();

	GLuint vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

GLint getUniformLocation(GLuint program, const std::string& name, std::unordered_map<std::string, GLint>& uniformCache)
{
    if (uniformCache.find(name) != uniformCache.end()) {
        return uniformCache[name];
    }
    
    GLint location = glGetUniformLocation(program, name.c_str());
    uniformCache[name] = location;
    return location;
}

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
	std::unordered_map<int, bool> *keyMap = as->keyMap;
	if (keyMap->find(key) != keyMap->end())
	{
		if (action == GLFW_PRESS)
		{
			(*keyMap)[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			(*keyMap)[key] = false;
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
	if (as->mouseHeld)
	{
		double dx = as->mouseEnterX - xpos;
        double dy = ypos - as->mouseEnterY;

        as->window.cx = as->dragStartCx + (8 * dx / as->window.w) / as->window.zoom; 
        as->window.cy = as->dragStartCy + (8 * dy / as->window.h) / as->window.zoom;
	}
}

void mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (ImGui::GetIO().WantCaptureMouse) return;
	ApplicationState* as = static_cast<ApplicationState*>(glfwGetWindowUserPointer(window));
	as->mouseHeld = (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS);
	glfwGetCursorPos(window, &as->mouseEnterX, &as->mouseEnterY);
	as->dragStartCx = as->window.cx;
    as->dragStartCy = as->window.cy;
}

void handleKeyMovement(ApplicationState &as)
{
	float moveSpeed = 0.05f / as.window.zoom;
	float dy = 0.0f, dx = 0.0f;
	if ((*as.keyMap)[GLFW_KEY_W]) dy += moveSpeed;
	if ((*as.keyMap)[GLFW_KEY_A]) dx -= moveSpeed;
	if ((*as.keyMap)[GLFW_KEY_S]) dy -= moveSpeed;
	if ((*as.keyMap)[GLFW_KEY_D]) dx += moveSpeed;
	as.window.cx += dx;
	as.window.cy += dy;
}

/*
	TODO Add support for both float and double in frag-shader
			-buttons to switch during execution, and text-indicator
	TODO Utilize compute shaders
*/

int main()
{
	GLFWwindow* window;

	int maxIterations = 128;
	int baseIterations = 128;
	float saturation = 1.0f;
	float brightness = 1.0f;
	
	std::unordered_map<int, bool> keyMap = {
		{GLFW_KEY_W, false},
		{GLFW_KEY_A, false},
		{GLFW_KEY_S, false},
		{GLFW_KEY_D, false}
	};
	ApplicationState applicationState = {{-0.5, 0, 2.0, 1080, 1080}, &keyMap};

	std::unordered_map<std::string, GLint> uniformCache;

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

	std::string vertexShader = readFile("../src/shaders/shader.vert");
	std::string fragmentShader = readFile("../src/shaders/shader.frag");
	GLuint program = CreateShaderProgram(vertexShader, fragmentShader);
	glUseProgram(program);

	glUniform1f(getUniformLocation(program, "u_zoom", uniformCache), applicationState.window.zoom);
	glUniform2i(getUniformLocation(program, "u_resolution", uniformCache), applicationState.window.w, applicationState.window.h);
	glUniform2f(getUniformLocation(program, "u_center", uniformCache), applicationState.window.cx, applicationState.window.cy);
	glUniform1i(getUniformLocation(program, "u_MAX_ITERATIONS", uniformCache), maxIterations);
	glUniform1i(getUniformLocation(program, "u_BASE_ITERATIONS", uniformCache), baseIterations);
	glUniform1f(getUniformLocation(program, "u_saturation", uniformCache), saturation);
	glUniform1f(getUniformLocation(program, "u_brightness", uniformCache), brightness);

	glEnable(GL_CULL_FACE);

	int maxFPS;

	{
		GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
		maxFPS = mode->refreshRate;
	}

	int targetFPS = maxFPS;
	float targetFrameTime = 1.0f / targetFPS;
	double lastTime = glfwGetTime();

	while (!glfwWindowShouldClose(window)) 
	{
		glfwPollEvents();

		handleKeyMovement(applicationState);
		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;

		if (deltaTime > targetFrameTime) {
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			if (ImGui::SliderInt("FPS Limit", &targetFPS, 1, maxFPS)) {
				targetFrameTime = 1.0f / targetFPS;  // Update the target frame time
			}
			if (ImGui::SliderInt("U_MAX_ITERATIONS", &maxIterations, 1, 1024))
			{
				glUniform1i(getUniformLocation(program, "u_MAX_ITERATIONS", uniformCache), maxIterations);
			}
			if (ImGui::SliderInt("U_BASE_ITERATIONS", &baseIterations, 1, 1024))
			{
				glUniform1i(getUniformLocation(program, "u_BASE_ITERATIONS", uniformCache), baseIterations);
			}

			glUniform1f(getUniformLocation(program, "u_zoom", uniformCache), applicationState.window.zoom);
			glUniform2i(getUniformLocation(program, "u_resolution", uniformCache), applicationState.window.w, applicationState.window.h);
			glUniform2f(getUniformLocation(program, "u_center", uniformCache), applicationState.window.cx, applicationState.window.cy);

			ImGui::BeginGroup();
			ImGui::Text("Color Controls");
			float sliderWidth = (ImGui::GetContentRegionAvail().x / 2.0f) - 10.0f;
			ImGui::PushItemWidth(sliderWidth);
			if(ImGui::SliderFloat("Saturation", &saturation, 0, 1))
			{
				glUniform1f(getUniformLocation(program, "u_saturation", uniformCache), saturation);
			}
			if(ImGui::SliderFloat("Brightness", &brightness, 0, 1))
			{
				glUniform1f(getUniformLocation(program, "u_brightness", uniformCache), brightness);
			}
			ImGui::PopItemWidth();
			ImGui::EndGroup();

			lastTime = currentTime;
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
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
	float xMin, xMax, yMin, yMax;
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

	int oldWidth, oldHeight;
	glfwGetWindowSize(window, &oldWidth, &oldHeight);
	int dw = width - oldWidth;
	int dh = height - oldHeight;

	// continue later with this
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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	WindowState* ws = static_cast<WindowState*>(glfwGetWindowUserPointer(window));
	static float zoomInFactor = 0.9;
	constexpr static float zoomOutFactor = 1.0 / 0.9;
	
	//Used to 
	/*	TODO in the future, zoom in and out from the point of the cursor.
	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);

	// Assuming we have a current cursor position (cursorX, cursorY)
	float scaleX = (xPos - ws->xMin) / (ws->xMax - ws->xMin);
	float scaleY = (yPos - ws->yMin) / (ws->yMax - ws->yMin);

	// Apply zoom factor
	xmin = cursorX - scaleX * (xmax - xmin) * zoomFactor;
	xmax = cursorX + (1 - scaleX) * (xmax - xmin) * zoomFactor;
	ymin = cursorY - scaleY * (ymax - ymin) * zoomFactor;
	ymax = cursorY + (1 - scaleY) * (ymax - ymin) * zoomFactor;

	// Adjust the other axis to maintain the aspect ratio
	float newAspect = (xmax - xmin) / (ymax - ymin);
	if (newAspect > 1.0) {
	    float delta = (xmax - xmin) / aspectRatio - (ymax - ymin);
	    ymin -= delta / 2;
	    ymax += delta / 2;
	} else {
	    float delta = (ymax - ymin) * aspectRatio - (xmax - xmin);
	    xmin -= delta / 2;
	    xmax += delta / 2;
	}*/
	std::cout << yoffset << std::endl;

	if (yoffset < 0)
	{
		ws->xMin *= zoomOutFactor;
		ws->xMax *= zoomOutFactor;
		ws->yMin *= zoomOutFactor;
		ws->yMax *= zoomOutFactor;
	}
	else
	{
		ws->xMin *= zoomInFactor;
		ws->xMax *= zoomInFactor;
		ws->yMin *= zoomInFactor;
		ws->yMax *= zoomInFactor;
	}
}

int main()
{
	GLFWwindow* window;

	int maxIterations = 128;
	WindowState ws = {-2.0f, 1.0f, -1.5f, 1.5f};

	std::unordered_map<std::string, GLint> uniformCache;

	if (!glfwInit()) {
		std::cout << "GLFW failed to initialize" << std::endl;
		return -1;
	}

	//glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
	window = glfwCreateWindow(1080, 1080, "My Window", NULL, NULL);
	glfwMakeContextCurrent(window);

	glfwSetWindowUserPointer(window, &ws);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, scroll_callback);

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

	float col[] = {
		1.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f
	};

	GLuint colors;
	glGenBuffers(1, &colors);
	glBindBuffer(GL_ARRAY_BUFFER, colors);
	glBufferData(GL_ARRAY_BUFFER, 9 * 2 * sizeof(float), col, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

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

	glUniform1f(getUniformLocation(program, "u_xMin", uniformCache), ws.xMin);
	glUniform1f(getUniformLocation(program, "u_xMax", uniformCache), ws.xMax);
	glUniform1f(getUniformLocation(program, "u_yMin", uniformCache), ws.yMin);
	glUniform1f(getUniformLocation(program, "u_yMax", uniformCache), ws.yMax);
	glUniform1i(getUniformLocation(program, "u_MAX_ITERATIONS", uniformCache), maxIterations);

	glEnable(GL_CULL_FACE);

	while (!glfwWindowShouldClose(window)) 
	{
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//ImGui::ShowDemoWindow();

		if (ImGui::SliderInt("U_MAX_ITERATIONS", &maxIterations, 1, 1024)) {
			glUniform1i(getUniformLocation(program, "u_MAX_ITERATIONS", uniformCache), maxIterations);
		}

		// TODO find a nice way to only update uniforms on zoom or translation
		glUniform1f(getUniformLocation(program, "u_xMin", uniformCache), ws.xMin);
		glUniform1f(getUniformLocation(program, "u_xMax", uniformCache), ws.xMax);
		glUniform1f(getUniformLocation(program, "u_yMin", uniformCache), ws.yMin);
		glUniform1f(getUniformLocation(program, "u_yMax", uniformCache), ws.yMax);

		glClear(GL_COLOR_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}
	
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
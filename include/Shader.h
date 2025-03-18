#ifndef SHADER
#define SHADER

#include <GL/glew.h>
#include <string>

class Shader
{
private:
	GLuint programID;
	GLuint loadShader(GLuint type, const std::string& source);
public:
	Shader(const std::string& vertexShader, const std::string& fragmentShader);
	~Shader();
	void use() const;
	GLuint getProgramID() const;

	//Uniforms
	void setUniform1f(const std::string& name, float value);
	void setUniform2f(const std::string& name, float val1, float val2);
	void setUniform1i(const std::string& name, int value);
	void setUniform2i(const std::string& name, int val1, int val2);
};

#endif
/*
glUniform1f(getUniformLocation(program, "u_zoom", uniformCache), applicationState.window.zoom);
glUniform2i(getUniformLocation(program, "u_resolution", uniformCache), applicationState.window.w, applicationState.window.h);
glUniform2f(getUniformLocation(program, "u_center", uniformCache), applicationState.window.cx, applicationState.window.cy);
glUniform1i(getUniformLocation(program, "u_MAX_ITERATIONS", uniformCache), maxIterations);
glUniform1i(getUniformLocation(program, "u_BASE_ITERATIONS", uniformCache), baseIterations);
glUniform1f(getUniformLocation(program, "u_saturation", uniformCache), saturation);
glUniform1f(getUniformLocation(program, "u_brightness", uniformCache), brightness);
glUniform2f(getUniformLocation(program, "u_julia_c", uniformCache), applicationState.juliaCx, applicationState.juliaCy);

 maybe
GLint getUniformLocation(GLuint program, const std::string& name, std::unordered_map<std::string, GLint>& uniformCache)
{
    if (uniformCache.find(name) != uniformCache.end()) {
        return uniformCache[name];
    }
    
    GLint location = glGetUniformLocation(program, name.c_str());
    uniformCache[name] = location;
    return location;
}

*/
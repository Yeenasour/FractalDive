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
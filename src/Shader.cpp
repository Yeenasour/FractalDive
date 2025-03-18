#include <Shader.h>
#include <FileUtils.h>

Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader)
{
	GLuint program = glCreateProgram();

	GLuint vs = loadShader(GL_VERTEX_SHADER, vertexShader);
	GLuint fs = loadShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	programID = program;
}

Shader::~Shader()
{
	if (programID != 0)
	{
		glDeleteProgram(programID);
		programID = 0;
	}
	
}

void Shader::use() const
{
	glUseProgram(programID);
}

GLuint Shader::getProgramID() const
{
	return programID;
}

GLuint Shader::loadShader(GLuint type, const std::string& source)
{
	GLuint shader = glCreateShader(type);
	std::string contents = FileUtils::readFile(source);
	const char* src = contents.c_str();
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	GLint res;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &res);
	if (res == GL_FALSE)
	{
		int len;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
		std::string msg(len, '\0');
		glGetShaderInfoLog(shader, len, &len, &msg[0]);
		std::cout << "Shader compilation failed! (" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << ")" << std::endl;
		std::cout << msg << std::endl;
		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

void Shader::setUniform1f(const std::string& name, float value)
{
	GLuint loc = glGetUniformLocation(programID, name.c_str());
	glUniform1f(loc, value);
}

void Shader::setUniform2f(const std::string& name, float val1, float val2)
{
	GLuint loc = glGetUniformLocation(programID, name.c_str());
	glUniform2f(loc, val1, val2);
}

void Shader::setUniform1i(const std::string& name, int value)
{
	GLuint loc = glGetUniformLocation(programID, name.c_str());
	glUniform1i(loc, value);
}

void Shader::setUniform2i(const std::string& name, int val1, int val2)
{
	GLuint loc = glGetUniformLocation(programID, name.c_str());
	glUniform2i(loc, val1, val2);
}

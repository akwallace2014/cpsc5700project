#include "Shader.h"
#include "GLXtras.h"

Shader::Shader(std::string vertexShaderFilePath, std::string fragShaderFilePath) {
	ID = 0;
	vertexShaderFile = vertexShaderFilePath;
	fragShaderFile = fragShaderFilePath;
}

void Shader::use() {
	ID = LinkProgramViaFile(vertexShaderFile.c_str(), fragShaderFile.c_str());
	if (!ID)
		printf("can't init shader programs: %s, %s\n", vertexShaderFile.c_str(), fragShaderFile.c_str());
}

GLuint Shader::getID() {
	return ID;
}
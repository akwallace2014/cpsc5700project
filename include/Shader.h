#ifndef SHADER_H
#define SHADER_H

#include <glad.h> 
#include <string>


class Shader {
public:
    Shader(std::string vertexShaderFilePath, std::string fragShaderFilePath);
    void use();
    GLuint getID();

private:
    GLuint ID;
    std::string vertexShaderFile, fragShaderFile;
};

#endif
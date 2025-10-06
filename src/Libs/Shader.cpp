#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader() : shaderID(0) {}

static std::string ReadFile(const char* path) {
    std::ifstream file(path);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

void Shader::CreateFromFiles(const char* vertexPath, const char* fragmentPath) {
    std::string vertCode = ReadFile(vertexPath);
    std::string fragCode = ReadFile(fragmentPath);

    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    const char* vCode = vertCode.c_str();
    glShaderSource(vert, 1, &vCode, nullptr);
    glCompileShader(vert);

    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fCode = fragCode.c_str();
    glShaderSource(frag, 1, &fCode, nullptr);
    glCompileShader(frag);

    shaderID = glCreateProgram();
    glAttachShader(shaderID, vert);
    glAttachShader(shaderID, frag);
    glLinkProgram(shaderID);

    glDeleteShader(vert);
    glDeleteShader(frag);
}

void Shader::UseShader() { glUseProgram(shaderID); }
GLuint Shader::GetShaderID() { return shaderID; }
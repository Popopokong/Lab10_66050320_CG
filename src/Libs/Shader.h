#pragma once
#include <string>
#include <GL/glew.h>

class Shader {
public:
    Shader();
    void CreateFromFiles(const char* vertexPath, const char* fragmentPath);
    void UseShader();
    GLuint GetShaderID();
private:
    GLuint shaderID;
};
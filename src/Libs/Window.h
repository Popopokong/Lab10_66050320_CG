#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window {
public:
    Window(int width, int height, int major, int minor);
    void initialise();
    int getBufferWidth();
    int getBufferHeight();
    bool getShouldClose();
    void swapBuffers();
private:
    GLFWwindow* mainWindow;
    int width, height;
    int bufferWidth, bufferHeight;
};
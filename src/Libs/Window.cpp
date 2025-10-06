#include "Window.h"
#include <iostream>

Window::Window(int width, int height, int major, int minor)
    : width(width), height(height), bufferWidth(0), bufferHeight(0), mainWindow(nullptr) {}

void Window::initialise() {
    if (!glfwInit()) {
        std::cerr << "GLFW init failed\n";
        return;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    mainWindow = glfwCreateWindow(width, height, "OpenGL Window", nullptr, nullptr);
    if (!mainWindow) {
        std::cerr << "GLFW window creation failed\n";
        glfwTerminate();
        return;
    }
    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);
    glfwMakeContextCurrent(mainWindow);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW init failed\n";
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return;
    }
}

int Window::getBufferWidth() { return bufferWidth; }
int Window::getBufferHeight() { return bufferHeight; }
bool Window::getShouldClose() { return glfwWindowShouldClose(mainWindow); }
void Window::swapBuffers() { glfwSwapBuffers(mainWindow); }
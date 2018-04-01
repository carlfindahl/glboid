#include "debug.h"

#include <iostream>

void glfwErrorCallback(int err, const char* msg)
{
    std::cout << "GLFW ERROR #" << err << ": " << msg << "\n";
}

void cppGLDebug(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    switch (severity)
    {
    case gl::DEBUG_SEVERITY_NOTIFICATION: break;
    case gl::DEBUG_SEVERITY_LOW: std::cout << "LOW: " << message << '\n'; break;
    case gl::DEBUG_SEVERITY_MEDIUM: std::cout << "MID: " << message << '\n'; break;
    case gl::DEBUG_SEVERITY_HIGH: std::cout << "HIH: " << message << '\n'; break;
    default: std::cout << "UKN: " << message << '\n'; break;
    }
}

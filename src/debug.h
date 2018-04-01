#include "gl_core4_5.hpp"

// GLFW Error/Debug Callback
void glfwErrorCallback(int err, const char* msg);

// OpenGL Debug Callback
void cppGLDebug(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                const GLchar* message, const void* userParam);

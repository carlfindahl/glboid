#include "flock.h"

#include <chrono>
#include <iostream>

#include "gl_core4_5.hpp"
#include "GLFW/glfw3.h"

// Global Window Instance
GLFWwindow* g_window = nullptr;

// The flock object
Flock g_flock(250);

// For GL Debug
unsigned unusedID = 0;

void glfwErrorCallback(int err, const char* msg)
{
    std::cout << "GLFW ERROR #" << err << ": " << msg << "\n";
}

void APIENTRY cppGLDebug(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                         const GLchar* message, const void* userParam)
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

const bool init()
{
    // Prepare error callback
    glfwSetErrorCallback(glfwErrorCallback);

    // Initialize GLFW
    if (auto res = glfwInit(); res != GLFW_TRUE)
    {
        std::cout << "Program has shit it's pants!\n";
        return false;
    }

    // Prepare Window Hints
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    // Create Window
    g_window = glfwCreateWindow(1000, 500, "Flocking GL", nullptr, nullptr);
    if (!g_window)
    {
        std::cout << "Failed to create window!\n";
        return false;
    }
    glfwMakeContextCurrent(g_window);

    // Load GL Functions with glLoadGen
    if (auto didLoad = gl::sys::LoadFunctions(); !didLoad)
    {
        std::cout << "Failed to used glLoadGen!\n";
        return false;
    }

    gl::Enable(gl::DEBUG_OUTPUT_SYNCHRONOUS);
    gl::DebugMessageCallback(cppGLDebug, nullptr);
    gl::DebugMessageControl(gl::DONT_CARE, gl::DONT_CARE, gl::DONT_CARE, 0, &unusedID, gl::TRUE_);

    // Output Context Version
    auto versionString = gl::GetString(gl::VERSION);
    std::cout << versionString << '\n';
    return true;
}

void terminate()
{
    glfwDestroyWindow(g_window);
    glfwTerminate();
}

void update()
{
    static auto lastUpdate = std::chrono::steady_clock::now();
    const auto now = std::chrono::steady_clock::now();
    const auto delta = std::chrono::duration<float>(lastUpdate - now).count();

    g_flock.update(delta);

    lastUpdate = now;
}

void draw()
{
    gl::Clear(gl::COLOR_BUFFER_BIT);
    g_flock.draw();
    glfwSwapBuffers(g_window);
}

int main()
{
    if (!init())
    {
        return 1;
    }

    g_flock.createDrawData();

    while (!glfwWindowShouldClose(g_window))
    {
        glfwPollEvents();
        update();
        draw();
    }

    terminate();

    return 0;
}

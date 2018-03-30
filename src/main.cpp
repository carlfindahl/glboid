#include "flock.h"

#include <chrono>
#include <iostream>

#include "gl_core4_5.hpp"
#include "GLFW/glfw3.h"

// Global Window Instance
GLFWwindow* g_window = nullptr;

// The flock object
Flock g_flock(250);

void glfwErrorCallback(int err, const char* msg)
{
    std::cout << "GLFW ERROR #" << err << ": " << msg << "\n";
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

    while (!glfwWindowShouldClose(g_window))
    {
        glfwPollEvents();
        update();
        draw();
    }

    terminate();

    return 0;
}

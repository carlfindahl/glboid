#include "detail.h"
#include "flock.h"

#include <chrono>
#include <iostream>

#include "gl_core4_5.hpp"
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

// Global Window Instance
GLFWwindow* g_window = nullptr;

// The flock object
Flock g_flock(666);

#ifndef NDEBUG
// For GL Debug
unsigned g_unusedID = 0;
#endif

// The OpenGL Shader Program used for all drawing
unsigned g_shaderProgram = 0;

bool init()
{
    // Prepare error callback
    glfwSetErrorCallback(glfwErrorCallback);

    // Initialize GLFW
    if (auto res = glfwInit(); res != GLFW_TRUE)
    {
        std::cout << "Failed to init GLFW!\n";
        return false;
    }

    // Prepare Window Hints
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
#ifndef NDEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

    // Create Window
    g_window = glfwCreateWindow(800, 800, "Flocking GL", nullptr, nullptr);
    if (!g_window)
    {
        std::cout << "Failed to create window!\n";
        return false;
    }

    // Set Window as current GL Context
    glfwMakeContextCurrent(g_window);
    glfwSwapInterval(1);

    // Load GL Functions with glLoadGen
    if (auto didLoad = gl::sys::LoadFunctions(); !didLoad)
    {
        std::cout << "Failed to load glLoadGen!\n";
        return false;
    }

    // Enable OpenGL Debug Callback only in Debug mode
#ifndef NDEBUG
    gl::Enable(gl::DEBUG_OUTPUT_SYNCHRONOUS);
    gl::DebugMessageCallback(cppGLDebug, nullptr);
    gl::DebugMessageControl(gl::DONT_CARE, gl::DONT_CARE, gl::DONT_CARE, 0, &g_unusedID, gl::TRUE_);
#endif

    // Output Context Version to Verify GL 4.5
    auto versionString = gl::GetString(gl::VERSION);
    std::cout << versionString << '\n';
    return true;
}

void terminate()
{
    gl::DeleteProgram(g_shaderProgram);
    glfwDestroyWindow(g_window);
    glfwTerminate();
}

void update()
{
    // Static time for last update
    static auto lastUpdate = std::chrono::steady_clock::now();

    // Set the new time
    const auto now = std::chrono::steady_clock::now();

    // Only update if the difference is > the update delta
    constexpr auto updateDelta = std::chrono::duration<float>(1.f / 120.f);
    if ((now - lastUpdate) > updateDelta)
    {
        g_flock.update(updateDelta.count());
        lastUpdate = now;
    }
}

void draw()
{
    gl::Clear(gl::COLOR_BUFFER_BIT);  // Clear buffer
    g_flock.draw();                   // Draw the Flock
    glfwSwapBuffers(g_window);        // Swap the back/front buffer to display
}

int main()
{
    // Init GLFW/OpenGL or exit on fail
    if (!init())
    {
        return 1;
    }

    // Prepare the shader and enable it
    makeShader();
    gl::UseProgram(g_shaderProgram);

    // Set the projection Uniform once, since program is always used
    const auto pmat = glm::ortho(0.f, 400.f, 400.f, 0.f);
    auto loc = gl::GetUniformLocation(g_shaderProgram, "projectionMatrix");
    gl::UniformMatrix4fv(loc, 1, gl::FALSE_, glm::value_ptr(pmat));

    // Create vertices / draw data for the Flock once
    g_flock.createDrawData();

    // Then loop until window should close
    while (!glfwWindowShouldClose(g_window))
    {
        glfwPollEvents();
        update();
        draw();
    }

    // Do some cleanup of GL / GLFW resources
    terminate();

    return 0;
}

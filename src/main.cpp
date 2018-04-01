#include "debug.h"
#include "flock.h"

#include <chrono>
#include <cstring>
#include <iostream>

#include "gl_core4_5.hpp"
#include "GLFW/glfw3.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

// Global Window Instance
GLFWwindow* g_window = nullptr;

// The flock object
Flock g_flock(666);

// For GL Debug
unsigned g_unusedID = 0;

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

void makeShader()
{
    // Create GL Shader Objects
    unsigned vert, frag;
    vert = gl::CreateShader(gl::VERTEX_SHADER);
    frag = gl::CreateShader(gl::FRAGMENT_SHADER);

    // Store Shader source code inline for practical purposes
    // (The executable does not rely on shader files)
    const char* vertSrc =
        R"(#version 450 core

layout (location=0) in vec4 aInstance_Position;
layout (location=1) in vec4 aPosition;
layout (location=2) in mat4 aInstance_Rotation;
layout (location=6) in vec4 aInstance_Velocity;

uniform mat4 projectionMatrix;

out vec4 vs_color;

const vec4 color_min = vec4(0.f, 1.f, .0f, 1.f);
const vec4 color_max = vec4(1.f, 0.f, .0f, 1.f);

void main()
{
        gl_Position = projectionMatrix * ((aInstance_Rotation * aPosition) + aInstance_Position);
        vs_color = smoothstep(color_min, color_max, vec4(length(aInstance_Velocity) / 10.f));
})";
    int vertLen = strlen(vertSrc);

    // Same with fragment shader source
    const char* fragSrc =
        R"(#version 450 core

layout(location=0) out vec4 color;

in vec4 vs_color;

void main()
{
        color = vec4(vs_color.rgb, 1.f);
})";
    int fragLen = strlen(fragSrc);

    // Set the sources and compile
    gl::ShaderSource(vert, 1, &vertSrc, &vertLen);
    gl::ShaderSource(frag, 1, &fragSrc, &fragLen);
    gl::CompileShader(vert);
    gl::CompileShader(frag);

    // Do extremely basic compile verification
    int didCompile;
    gl::GetShaderiv(vert, gl::COMPILE_STATUS, &didCompile);
    if (!didCompile)
        std::cout << "Failed Vertex!\n";
    gl::GetShaderiv(frag, gl::COMPILE_STATUS, &didCompile);
    if (!didCompile)
        std::cout << "Failed Fragment!\n";

    // Create and link program with the two shaders
    g_shaderProgram = gl::CreateProgram();
    gl::AttachShader(g_shaderProgram, vert);
    gl::AttachShader(g_shaderProgram, frag);
    gl::LinkProgram(g_shaderProgram);

    // Do an equally basic link verification
    gl::GetProgramiv(g_shaderProgram, gl::LINK_STATUS, &didCompile);
    if (!didCompile)
        std::cout << "Failed Link!\n";

    // Clean up shaders as they are no longer needed
    gl::DeleteShader(vert);
    gl::DeleteShader(frag);
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

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

unsigned shaderProgram;

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
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

    // Create Window
    g_window = glfwCreateWindow(800, 800, "Flocking GL", nullptr, nullptr);
    if (!g_window)
    {
        std::cout << "Failed to create window!\n";
        return false;
    }
    glfwMakeContextCurrent(g_window);
    glfwSwapInterval(1);

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

void makeShader()
{
    unsigned vert, frag;

    vert = gl::CreateShader(gl::VERTEX_SHADER);
    frag = gl::CreateShader(gl::FRAGMENT_SHADER);

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

    const char* fragSrc =
R"(#version 450 core

layout(location=0) out vec4 color;

in vec4 vs_color;

void main()
{
        color = vec4(vs_color.rgb, 1.f);
})";
    int fragLen = strlen(fragSrc);

    gl::ShaderSource(vert, 1, &vertSrc, &vertLen);
    gl::ShaderSource(frag, 1, &fragSrc, &fragLen);
    gl::CompileShader(vert);
    gl::CompileShader(frag);

    int didCompile;
    gl::GetShaderiv(vert, gl::COMPILE_STATUS, &didCompile);
    if (!didCompile)
        std::cout << "Failed Vertex!\n";
    gl::GetShaderiv(frag, gl::COMPILE_STATUS, &didCompile);
    if (!didCompile)
        std::cout << "Failed Fragment!\n";

    shaderProgram = gl::CreateProgram();
    gl::AttachShader(shaderProgram, vert);
    gl::AttachShader(shaderProgram, frag);
    gl::LinkProgram(shaderProgram);

    gl::GetProgramiv(shaderProgram, gl::LINK_STATUS, &didCompile);
    if (!didCompile)
        std::cout << "Failed Link!\n";

    gl::DeleteShader(vert);
    gl::DeleteShader(frag);
}

void terminate()
{
    gl::DeleteProgram(shaderProgram);
    glfwDestroyWindow(g_window);
    glfwTerminate();
}

void update()
{
    static auto lastUpdate = std::chrono::steady_clock::now();
    const auto now = std::chrono::steady_clock::now();

    if ((now - lastUpdate) > std::chrono::duration<float>(1.f / 120.f))
    {
        g_flock.update(1.f / 60.f);
        lastUpdate = now;
    }
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

    makeShader();

    gl::UseProgram(shaderProgram);
    const auto pmat = glm::ortho(0.f, 400.f, 400.f, 0.f);
    auto loc = gl::GetUniformLocation(shaderProgram, "projectionMatrix");
    gl::UniformMatrix4fv(loc, 1, gl::FALSE_, glm::value_ptr(pmat));

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

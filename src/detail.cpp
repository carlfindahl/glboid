#include "detail.h"

#include <cstring>
#include <iostream>

extern unsigned g_shaderProgram;

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

#include "flock.h"

#include <algorithm>
#include <iostream>
#include <random>
#include <cmath>

#include "glm/gtc/matrix_transform.hpp"
#include "gl_core4_5.hpp"
#include "GLFW/glfw3.h"

extern GLFWwindow* g_window;

Flock::Flock(const std::size_t count) : m_positions(count), m_velocities(count), m_rotations(count), m_count(count)
{
    std::random_device seed;
    std::mt19937 generator(seed());
    std::uniform_real_distribution<float> rng(0.f, 1.f);

    // Initialize Positions
    for (auto& p : m_positions)
    {
        p.x = rng(generator) * 800.f;
        p.y = rng(generator) * 800.f;
    }

    // Initialize Velocities
    for (auto& v : m_velocities)
    {
        v.x = rng(generator) * 0.4f;
        v.y = rng(generator) * 0.4f;
    }

    for(auto& r : m_rotations){
        r = glm::mat4(1.f);
    }
}

Flock::~Flock()
{
    gl::DeleteVertexArrays(1, &m_vao);
    gl::DeleteBuffers(1, &m_vbo);
    gl::DeleteBuffers(1, &m_tvbo);
}

void Flock::createDrawData()
{
    // Per Instance Position Buffer
    gl::CreateBuffers(1, &m_vbo);
    gl::NamedBufferStorage(m_vbo, sizeof(glm::vec2) * m_count, m_positions.data(),
                           gl::DYNAMIC_STORAGE_BIT);

    // Per Instance Rotation Buffer
    gl::CreateBuffers(1, &m_rvbo);
    gl::NamedBufferStorage(m_rvbo, sizeof(glm::mat4) * m_count, m_rotations.data(),
                           gl::DYNAMIC_STORAGE_BIT);

    // Triangle Buffer
    gl::CreateBuffers(1, &m_tvbo);
    float data[6] = {4.f, 4.f, 4.f, -4.f, -6.f, 0.f};
    gl::NamedBufferStorage(m_tvbo, sizeof(data), data, 0);

    gl::CreateVertexArrays(1, &m_vao);

    // Attrib 0, Binding 0 - Per Instance Position
    gl::VertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, sizeof(glm::vec2));
    gl::VertexArrayAttribBinding(m_vao, 0, 0);
    gl::VertexArrayAttribFormat(m_vao, 0, 2, gl::FLOAT, gl::FALSE_, 0);
    gl::VertexArrayBindingDivisor(m_vao, 0, 1);
    gl::EnableVertexArrayAttrib(m_vao, 0);

    // Attrib 1, Binding 1 - The Triangle
    gl::VertexArrayVertexBuffer(m_vao, 1, m_tvbo, 0, sizeof(float) * 2);
    gl::VertexArrayAttribFormat(m_vao, 1, 2, gl::FLOAT, gl::FALSE_, 0);
    gl::VertexArrayAttribBinding(m_vao, 1, 1);
    gl::EnableVertexArrayAttrib(m_vao, 1);

    // Attrib 2-5, Binding 2 - Rotations
    gl::VertexArrayVertexBuffer(m_vao, 2, m_rvbo, 0, sizeof(glm::mat4));
    gl::VertexArrayAttribFormat(m_vao, 2, 4, gl::FLOAT, gl::FALSE_, 0);
    gl::VertexArrayAttribFormat(m_vao, 3, 4, gl::FLOAT, gl::FALSE_, 16);
    gl::VertexArrayAttribFormat(m_vao, 4, 4, gl::FLOAT, gl::FALSE_, 32);
    gl::VertexArrayAttribFormat(m_vao, 5, 4, gl::FLOAT, gl::FALSE_, 48);
    gl::VertexArrayAttribBinding(m_vao, 2, 2);
    gl::VertexArrayAttribBinding(m_vao, 3, 2);
    gl::VertexArrayAttribBinding(m_vao, 4, 2);
    gl::VertexArrayAttribBinding(m_vao, 5, 2);
    gl::VertexArrayBindingDivisor(m_vao, 2, 1);
    gl::VertexArrayBindingDivisor(m_vao, 3, 1);
    gl::VertexArrayBindingDivisor(m_vao, 4, 1);
    gl::VertexArrayBindingDivisor(m_vao, 5, 1);
    gl::EnableVertexArrayAttrib(m_vao, 2);
    gl::EnableVertexArrayAttrib(m_vao, 3);
    gl::EnableVertexArrayAttrib(m_vao, 4);
    gl::EnableVertexArrayAttrib(m_vao, 5);
}

void Flock::update(const float dt)
{
    double x, y;
    glfwGetCursorPos(g_window, &x, &y);

    for (int i = 0; i != m_count; ++i)
    {
        glm::vec2 v1, v2, v3, v4;
        v4 = (m_positions[i] - glm::vec2(static_cast<float>(x), static_cast<float>(y))) * 0.05f;

        std::vector<std::size_t> neighbours;

        for (int j = 0; j != m_count; ++j)
        {
            if (j != i && glm::length(m_positions[j] - m_positions[i]) < 80.f)
            {
                neighbours.push_back(j);
            }
        }

        for (auto& j : neighbours)
        {
            // Cohesion
            v1 += m_positions[j];
            v2 += m_velocities[j];

            // Avoidance
            if (glm::length(m_positions[j] - m_positions[i]) < 6.f)
            {
                v3 += (m_positions[j] - m_positions[i]);
            }
        }

        v1 *= 1.f / neighbours.size();
        v1 = (v1 - m_positions[i]) * 0.01f;

        v2 *= 1.f / neighbours.size();
        v2 = (v2 - m_velocities[i]) * 0.12f;

        m_velocities[i] += (v1 + v2 + v3 + v4);
        if (glm::length(m_velocities[i]) > 100.f)
        {
            m_velocities[i] = glm::normalize(m_velocities[i]) * 100.f;
        }
        m_positions[i] += m_velocities[i] * dt;

        m_rotations[i] = glm::rotate(glm::mat4(1.f), std::atan2(m_velocities[i].y, m_velocities[i].x), glm::vec3(0.f, 0.f, 1.f));
    }

    gl::NamedBufferSubData(m_vbo, 0, sizeof(glm::vec2) * m_count, m_positions.data());
    gl::NamedBufferSubData(m_rvbo, 0, sizeof(glm::mat4) * m_count, m_rotations.data());
}

void Flock::draw()
{
    gl::BindVertexArray(m_vao);
    gl::DrawArraysInstanced(gl::TRIANGLES, 0, 3, m_count);
}

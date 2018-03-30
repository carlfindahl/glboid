#include "flock.h"

#include <random>
#include <iostream>

#include "gl_core4_5.hpp"

Flock::Flock(const std::size_t count) : m_positions(count), m_velocities(count), m_count(count)
{
    std::random_device seed;
    std::mt19937 generator(seed());
    std::uniform_real_distribution<float> rng(0.f, 1.f);

    // Initialize Positions
    for (auto& p : m_positions)
    {
        p.x = rng(generator) * 400.f;
        p.y = rng(generator) * 400.f;
    }

    // Initialize Velocities
    for (auto& v : m_velocities)
    {
        v.x = rng(generator) * 20.f;
        v.y = rng(generator) * 20.f;
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
    gl::NamedBufferStorage(m_vbo, sizeof(glm::vec2) * m_count, m_positions.data(), 0);

    // Triangle Buffer
    gl::CreateBuffers(1, &m_tvbo);
    float data[6] = { -4.f, -4.f, 4.f, -4.f, 0.f, 4.f };
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
}

void Flock::update(const float dt) {

}

void Flock::draw()
{
    gl::BindVertexArray(m_vao);
    gl::DrawArraysInstanced(gl::TRIANGLES, 0, 3, m_count);
}

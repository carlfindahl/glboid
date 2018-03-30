#include "flock.h"

#include <random>

#include "gl_core4_5.hpp"

Flock::Flock(const std::size_t count) : m_positions(count), m_velocities(count), m_count(count)
{
    std::random_device seed;
    std::mt19937 generator(seed());
    std::uniform_real_distribution<float> rng(0.f, 1.f);

    // Initialize Positions
    for (auto& p : m_positions)
    {
        p.x = rng(generator) * 1000.f;
        p.y = rng(generator) * 500.f;
    }

    // Initialize Velocities
    for (auto& v : m_velocities)
    {
        v.x = rng(generator) * 20.f;
        v.y = rng(generator) * 20.f;
    }

    createDrawData();
}

Flock::~Flock()
{
    gl::DeleteBuffers(1, &m_vbo);
    gl::DeleteVertexArrays(1, &m_vao);
}

void Flock::createDrawData()
{
    gl::CreateBuffers(1, &m_vbo);
    gl::NamedBufferStorage(m_vbo, sizeof(glm::vec2) * m_count, m_positions.data(), 0);

    gl::CreateVertexArrays(1, &m_vao);
    gl::VertexArrayVertexBuffer(m_vao, 0, m_vbo, 0, sizeof(glm::vec2));
    gl::VertexArrayAttribBinding(m_vao, 0, 0);
    gl::VertexArrayAttribFormat(m_vao, 0, 2, gl::FLOAT, gl::FALSE_, 0);
}

void Flock::update(const float dt) {}

void Flock::draw() {

}

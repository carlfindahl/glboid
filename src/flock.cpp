#include "flock.h"

#include <random>

Flock::Flock(const std::size_t count) : m_positions(count), m_velocities(count)
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
}

void Flock::update(const float dt) {}

void Flock::draw() {}

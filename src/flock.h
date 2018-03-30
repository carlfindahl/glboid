#ifndef FLOCK_H
#define FLOCK_H

#include <vector>

#include "glm/glm.hpp"

class Flock
{
private:
    // Positions
    std::vector<glm::vec2> m_positions;

    // Velocities
    std::vector<glm::vec2> m_velocities;

    // Number of boids
    unsigned m_count;

    // Vertex array for boids
    unsigned m_vao;

    // Vertex buffer for boids and for the triangle
    unsigned m_vbo, m_tvbo;

public:
    Flock(const std::size_t count);

    ~Flock();

    // Create GL Draw data
    void createDrawData();

    // Update the flock
    void update(const float dt);

    // Do all necessary GL work to be draw the Flock
    void draw();
};

#endif // FLOCK_H
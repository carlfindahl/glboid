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

    // Rotation Matrices
    std::vector<glm::mat4> m_rotations;

    // Number of boids
    unsigned m_count;

    // Vertex array for boid drawing
    unsigned m_vao;

    // Vertex Buffer Objects
    // pvbo - Position Buffer Object
    // tvbo - Triangle Buffer Object
    // rvbo - Rotation Buffer Object
    // vvbo - Velocity Buffer Object
    unsigned m_pvbo, m_tvbo, m_rvbo, m_vvbo;

public:
    // Flocks are constructed with count boids
    Flock(const std::size_t count);

    // No copy-move ctor/assignment
    Flock(const Flock&) = delete;
    Flock& operator=(const Flock&) = delete;
    Flock& operator=(Flock&&) = delete;
    Flock(Flock&&) = delete;

    // Clean up resources
    ~Flock();

    // Create GL Draw data
    void createDrawData();

    // Update the flock
    void update(const float dt);

    // Do all necessary GL work to draw the Flock
    void draw();
};

#endif // FLOCK_H

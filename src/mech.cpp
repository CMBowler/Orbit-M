#include "../include/mech.hpp"


// Function to calculate gravitational force between two bodies
Vector3 calculateForce(const Body& a, const Body& b) {
    Vector3 direction = b.position - a.position;
    float distance = direction.magnitude();
    distance = std::max(distance, 0.1f); // Avoid division by zero or very small distances
    float force = G * a.mass * b.mass / (distance * distance);
    return direction.normalized() * force;
}

// Update simulation
void updateBodies(std::vector<Body>& bodies) {
    std::vector<Vector3> forces(bodies.size(), Vector3(0.0f, 0.0f, 0.0f));

    // Compute forces for each pair of bodies
    for (size_t i = 0; i < bodies.size(); ++i) {
        for (size_t j = i + 1; j < bodies.size(); ++j) {
            Vector3 force = calculateForce(bodies[i], bodies[j]);
            forces[i] = forces[i] + force;
            forces[j] = forces[j] - force; // Newton's third law
        }
    }

    // Update positions and velocities
    for (size_t i = 0; i < bodies.size(); ++i) {
        Vector3 acceleration = forces[i] / bodies[i].mass;
        bodies[i].velocity = bodies[i].velocity + acceleration * TIME_STEP;
        bodies[i].position = bodies[i].position + bodies[i].velocity * TIME_STEP;
        bodies[i].updateTail();
    }
}

// Function to compute the distance between two 3D points
float computeDistance(const Vector3& a, const Vector3& b) {
    Vector3 diff = b - a;
    return diff.magnitude();
}
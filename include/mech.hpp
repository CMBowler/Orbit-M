#ifndef MECH_HPP_
#define MECH_HPP_

#include <vector>
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>

// Constants
const float G = 6.67430e-11f;  // Gravitational constant (scaled for visualization)
const float TIME_STEP = 0.01f; // Time step for simulation (seconds)
const size_t MAX_TAIL_LENGTH = 100; // Maximum number of points in the tail
// Simulation parameters
const float RENDEZVOUS_DISTANCE = 25.0f; // Threshold for slowing simulation
const float SLOWDOWN_FACTOR = 0.1f;      // Factor by which to slow the simulation


// 3D Vector Structure
struct Vector3 {
    float x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    Vector3 operator/(float scalar) const {
        return Vector3(x / scalar, y / scalar, z / scalar);
    }

    float magnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    Vector3 normalized() const {
        float mag = magnitude();
        return (mag > 0) ? *this / mag : Vector3(0, 0, 0);
    }
};

// Body Structure
struct Body {
    Vector3 position;
    Vector3 velocity;
    float mass;
    sf::CircleShape shape;
    std::vector<Vector3> tail; // History of positions for the tail

    Body(float x, float y, float z, float vx, float vy, float vz, float mass, float radius, sf::Color color)
        : position(x, y, z), velocity(vx, vy, vz), mass(mass) {
        shape.setRadius(radius);
        shape.setOrigin(radius, radius); // Center the shape
        shape.setFillColor(color);
    }

    // Add current position to the tail
    void updateTail() {
        tail.push_back(position);
        if (tail.size() > MAX_TAIL_LENGTH) {
            tail.erase(tail.begin()); // Remove the oldest point if the tail is too long
        }
    }
};

Vector3 calculateForce(const Body& a, const Body& b);

void updateBodies(std::vector<Body>& bodies);

float computeDistance(const Vector3& a, const Vector3& b);

#endif // MECH_HPP_
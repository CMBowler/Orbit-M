#include <SFML/Graphics.hpp>
#include <vector>
#include "../include/mech.hpp"


// Project 3D position to 2D for rendering
sf::Vector2f projectTo2D(const Vector3& position, float screenWidth, float screenHeight, float scale) {
    float perspective = 500.0f / (500.0f + position.z); // Simple perspective scaling
    return sf::Vector2f(
        screenWidth / 2 + position.x * scale * perspective,
        screenHeight / 2 - position.y * scale * perspective // Invert y-axis for correct orientation
    );
}

// Function to project 3D position onto a 2D plane (for different views)
sf::Vector2f projectToPlane(const Vector3& position, char plane, float scale) {
    switch (plane) {
        case 'y': // Top-down view
            return sf::Vector2f(position.x * scale, position.y * scale);
        case 'z': // Side view
            return sf::Vector2f(position.x * scale, position.z * scale);
        default:
            return sf::Vector2f(0, 0); // Default case
    }
}

// Draw tails for all bodies
void drawTails(sf::RenderWindow& window, const std::vector<Body>& bodies, char plane, float scale) {
    for (const auto& body : bodies) {
        sf::VertexArray tail(sf::LineStrip, body.tail.size());
        for (size_t i = 0; i < body.tail.size(); ++i) {
            tail[i].position = projectToPlane(body.tail[i], plane, scale);
            tail[i].color = body.shape.getFillColor();
            float alpha = 255 * (1.0f - static_cast<float>(i) / body.tail.size());
            tail[i].color.a = static_cast<sf::Uint8>(alpha);
        }
        window.draw(tail);
    }
}
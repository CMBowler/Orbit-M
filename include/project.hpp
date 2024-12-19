#ifndef PROJECT_HPP_
#define PROJECT_HPP_


sf::Vector2f projectTo2D(const Vector3& position, float screenWidth, float screenHeight, float scale);

sf::Vector2f projectToPlane(const Vector3& position, char plane, float scale);

void drawTails(sf::RenderWindow& window, const std::vector<Body>& bodies, char plane, float scale);

#endif // PROJECT_HPP_
#include <iostream>


#include "../include/mech.hpp"
#include "../include/project.hpp"
#include "../include/loadScenario.hpp"

// Use the JSON library
using json = nlohmann::json;

// Application states
enum AppState { MENU, SIMULATION };

// Function to check if the mouse is within a view's bounds
bool isMouseOverView(const sf::RenderWindow& window, const sf::View& view, const sf::Vector2i& mousePos) {
    sf::FloatRect viewRect(view.getViewport().left * window.getSize().x,
                           view.getViewport().top * window.getSize().y,
                           view.getViewport().width * window.getSize().x,
                           view.getViewport().height * window.getSize().y);
    return viewRect.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
}

int main() {
    sf::RenderWindow menuWindow(sf::VideoMode(800, 600), "Menu");
    sf::RenderWindow simulationWindowTopDown(sf::VideoMode(800, 600), "Top-Down View");
    sf::RenderWindow simulationWindowSide(sf::VideoMode(800, 600), "Side View");
    simulationWindowTopDown.setFramerateLimit(60);
    simulationWindowSide.setFramerateLimit(60);

    //menuWindow.setVisible(true);
    //simulationWindowTopDown.setVisible(false);
    //simulationWindowSide.setVisible(false);

    AppState appState = MENU; // Start in the menu state
    std::vector<Body> bodies;
    std::vector<std::pair<std::string, std::vector<Body>>> scenarios;

    try {
        scenarios = loadAllScenarios("rsc/scenarios.json");
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }


    // Menu font and items
    sf::Font font;
    if (!font.loadFromFile("rsc/arial.ttf")) {
        throw std::runtime_error("Failed to load font");
        return EXIT_FAILURE;
    }
    sf::Text title("Select a Scenario", font, 36);
    title.setPosition(200, 50);

    std::vector<sf::Text> menuItems;
    for (size_t i = 0; i < scenarios.size(); ++i) {
        sf::Text item(scenarios[i].first, font, 24);
        item.setPosition(200, 150 + i * 50);
        menuItems.push_back(item);
    }

    size_t selectedScenario = 0; // Track the current selection
    bool isPaused = false;

    float simulationWindowTopDownWidth = simulationWindowTopDown.getSize().x;
    float simulationWindowTopDownHeight = simulationWindowTopDown.getSize().y;

    float simulationWindowSideWidth = simulationWindowSide.getSize().x;
    float simulationWindowSideHeight = simulationWindowSide.getSize().y;

    float scale = 1.0f; // Scale factor for projections
    sf::View topDownView(sf::FloatRect(-400, -300, 800, 600));
    sf::View sideView(sf::FloatRect(-400, -300, 800, 600));

    bool isDragging = false;
    sf::Vector2i dragStartPosition;
    sf::View* activeView = nullptr;

    Body* bodyA; // Reference to Body A
    Body* bodyB; // Reference to Body B

    float prevDistance = 0.0f;
    float currentDistance = 0;
    std::vector<Vector3> maximaPositions; // Store positions of local maxima

    bool isSlowingDown = false;
    sf::Clock slowdownClock; // To time the slowdown duration

    // Font and text for displaying slowdown message
    //sf::Font font;
    //if (!font.loadFromFile("/home/cbowler/projects/n-body/text/arial.ttf")) {
    //    return EXIT_FAILURE; // Handle font loading error
    //}

    //sf::Text slowdownText("Simulation slowing for rendezvous", font, 20);
    //slowdownText.setFillColor(sf::Color::White);
    //slowdownText.setPosition(10, 10);

    // Simulation loop
    while (menuWindow.isOpen() || simulationWindowTopDown.isOpen() || simulationWindowSide.isOpen()) {
        if (appState == MENU) {
            sf::Event event;
            while (menuWindow.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    menuWindow.close();
                    simulationWindowTopDown.close();
                    simulationWindowSide.close();
                }

                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Up) {
                        selectedScenario = (selectedScenario + scenarios.size() - 1) % scenarios.size();
                    } else if (event.key.code == sf::Keyboard::Down) {
                        selectedScenario = (selectedScenario + 1) % scenarios.size();
                    } else if (event.key.code == sf::Keyboard::Enter) {
                        bodies = scenarios[selectedScenario].second;
                        bodyA = &bodies[0]; // Reference to Body A
                        bodyB = &bodies[3]; // Reference to Body B
                        appState = SIMULATION;
                        menuWindow.setVisible(false);
                        simulationWindowTopDown.setVisible(true);
                        simulationWindowSide.setVisible(true);
                    }
                }
            }

            // Render the menu
            menuWindow.clear();
            menuWindow.draw(title);
            for (size_t i = 0; i < menuItems.size(); ++i) {
                if (i == selectedScenario) {
                    menuItems[i].setFillColor(sf::Color::Red);
                } else {
                    menuItems[i].setFillColor(sf::Color::White);
                }
                menuWindow.draw(menuItems[i]);
            }
            menuWindow.display();
        }

        if (appState == SIMULATION) {
            sf::Event event;
            while (simulationWindowTopDown.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    simulationWindowTopDown.close();
                    simulationWindowSide.close();
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                    appState = MENU;
                    simulationWindowTopDown.setVisible(false);
                    simulationWindowSide.setVisible(false);
                    menuWindow.setVisible(true);
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                    isPaused = !isPaused; // Toggle pause
                }

                sf::Vector2i mousePos = sf::Mouse::getPosition(simulationWindowTopDown);

                if (event.type == sf::Event::MouseWheelScrolled) {
                    topDownView.zoom(event.mouseWheelScroll.delta > 0 ? 0.9f : 1.1f);
                }

                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    isDragging = true;
                    dragStartPosition = mousePos;
                    activeView = &topDownView;
                }

                if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                    isDragging = false;
                    activeView = nullptr;
                }
            }

            while (simulationWindowSide.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    simulationWindowSide.close();
                    simulationWindowTopDown.close();
                }

                sf::Vector2i mousePos = sf::Mouse::getPosition(simulationWindowSide);

                if (event.type == sf::Event::MouseWheelScrolled) {
                    sideView.zoom(event.mouseWheelScroll.delta > 0 ? 0.9f : 1.1f);
                }

                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    isDragging = true;
                    dragStartPosition = mousePos;
                    activeView = &sideView;
                }

                if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                    isDragging = false;
                    activeView = nullptr;
                }
            }

            // Update simulation only if not paused
            if (!isPaused) {
                updateBodies(bodies);
            }

            // Handle panning for the active window
            if (isDragging && activeView) {
                sf::RenderWindow* activeWindow = (activeView == &topDownView) ? &simulationWindowTopDown : &simulationWindowSide;
                sf::Vector2i currentMousePosition = sf::Mouse::getPosition(*activeWindow);
                sf::Vector2f delta = activeWindow->mapPixelToCoords(dragStartPosition, *activeView) -
                                    activeWindow->mapPixelToCoords(currentMousePosition, *activeView);
                activeView->move(delta);
                dragStartPosition = currentMousePosition;
            }

            // Check for rendezvous (close approach)
            isSlowingDown = false;
            for (size_t i = 0; i < bodies.size(); ++i) {
                for (size_t j = i + 1; j < bodies.size(); ++j) {
                    float distance = computeDistance(bodies[i].position, bodies[j].position);
                    if (distance < RENDEZVOUS_DISTANCE) {
                        isSlowingDown = true;
                        break;
                    }
                }
                if (isSlowingDown) break;
            }

            // Slow down the simulation if rendezvous is detected
            int t_step = TIME_STEP;
            if (isSlowingDown) {
                t_step *= SLOWDOWN_FACTOR; // Pause briefly
            }

            // Update simulation
            updateBodies(bodies);

            // Detect local maxima for bodyB relative to bodyA
            float nextDistance = computeDistance(bodyA->position, bodyB->position);
            if (prevDistance < currentDistance && currentDistance > nextDistance) {
                maximaPositions.push_back(bodyB->position); // Save position at local maximum
            }
            prevDistance = currentDistance;
            currentDistance = nextDistance;



            // Render the top-down view
            simulationWindowTopDown.clear();

            // Draw tails
            drawTails(simulationWindowTopDown, bodies, 'y', scale);

            // Draw local maxima marks
            for (const auto& position : maximaPositions) {
                sf::Vector2f projected = projectToPlane(position, 'y', scale);
                sf::CircleShape marker(2.0f); // Small white dot
                marker.setFillColor(sf::Color::White);
                marker.setOrigin(2.0f, 2.0f);
                marker.setPosition(projected);
                simulationWindowTopDown.draw(marker);
            }

            simulationWindowTopDown.setView(topDownView);
            for (auto& body : bodies) {
                sf::Vector2f projected = projectToPlane(body.position, 'y', scale);
                body.shape.setPosition(projected);
                simulationWindowTopDown.draw(body.shape);
            }
            simulationWindowTopDown.display();

            // Render the side view
            simulationWindowSide.clear();

            // Draw tails
            drawTails(simulationWindowSide, bodies, 'z', scale);

            // Draw local maxima marks
            for (const auto& position : maximaPositions) {
                sf::Vector2f projected = projectToPlane(position, 'z', scale);
                sf::CircleShape marker(2.0f); // Small white dot
                marker.setFillColor(sf::Color::White);
                marker.setOrigin(2.0f, 2.0f);
                marker.setPosition(projected);
                simulationWindowSide.draw(marker);
            }

            simulationWindowSide.setView(sideView);
            for (auto& body : bodies) {
                sf::Vector2f projected = projectToPlane(body.position, 'z', scale);
                body.shape.setPosition(projected);
                simulationWindowSide.draw(body.shape);
            }
            simulationWindowSide.display();

        }
    }

    return EXIT_SUCCESS;
}

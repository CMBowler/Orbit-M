#include "../include/loadScenario.hpp"

// Use the JSON library
using json = nlohmann::json;

// Function to load all scenarios from a single JSON file
std::vector<std::pair<std::string, std::vector<Body>>> loadAllScenarios(const std::string& filename) {
    std::vector<std::pair<std::string, std::vector<Body>>> scenarios;
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open scenarios file: " + filename);
    }

    json data;
    file >> data;

    for (const auto& scenarioData : data["scenarios"]) {
        std::vector<Body> bodies;
        for (const auto& bodyData : scenarioData["bodies"]) {
            bodies.emplace_back(
                bodyData["x"], bodyData["y"], bodyData["z"],
                bodyData["vx"], bodyData["vy"], bodyData["vz"],
                bodyData["mass"], bodyData["radius"],
                sf::Color(bodyData["color"][0], bodyData["color"][1], bodyData["color"][2])
            );
        }
        scenarios.emplace_back(scenarioData["name"], bodies);
    }

    return scenarios;
}
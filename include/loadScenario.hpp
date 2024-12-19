#ifndef LOAD_SCENARIO_HPP_
#define LOAD_SCENARIO_HPP_

#include "mech.hpp"
#include "json.hpp"

std::vector<std::pair<std::string, std::vector<Body>>> loadAllScenarios(const std::string& filename);

#endif // LOAD_SCENARIO_HPP_
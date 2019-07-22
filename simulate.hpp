#ifndef ENERGYPLUSPARSER_SIMULATE_HPP
#define ENERGYPLUSPARSER_SIMULATE_HPP

#include <string>
#include "../generation/generation.hpp"

void simulate(Input const & user_input,
              std::string const &output_directory,
              std::string const &energy_plus_directory);

#endif //ENERGYPLUSPARSER_SIMULATE_HPP

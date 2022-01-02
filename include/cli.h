#include <boost/program_options.hpp>
#include "sim_settings.h"

namespace po = boost::program_options;

// parse user options
// true if options are parsed and valid
// false otherwise
bool parse_cli_args(int argc, char** argv, po::variables_map& vm, Simulation::SimSettings& settings);
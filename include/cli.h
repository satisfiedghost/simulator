#include "sim_settings.h"

#include <boost/program_options.hpp>

namespace po = boost::program_options;
namespace Cli {

// parse user options
// true if options are parsed and valid
// false otherwise
bool parse_cli_args(int argc, char** argv, po::variables_map& vm, Simulation::SimSettings& settings);

} // namespace Cli
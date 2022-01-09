#include "sim_settings.h"
#include "util/status.h"

#include <boost/program_options.hpp>

namespace po = boost::program_options;
namespace Cli {

// parse user options
// Status::Success if options are valid
// Status::Failure if not
// Status::None to display help and exit normally
template <typename Vt>
Status parse_cli_args(int argc, char** argv, po::variables_map& vm, Simulation::SimSettings<Vt>& settings);

} // namespace Cli
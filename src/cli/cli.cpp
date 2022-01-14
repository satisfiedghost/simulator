#include "cli.h"

#include <iostream>

namespace Cli {

template <typename V>
Status parse_cli_args(int argc, char** argv, po::variables_map& vm, Simulation::SimSettings<typename V::vector_t>& settings) {
  typedef typename V::vector_t vector_t;
  po::options_description desc("Allowed options");
  desc.add_options()
      ("help", "Display this help message.")
      ("pcount",
        po::value<size_t>(&settings.number_particles)->default_value(Simulation::DefaultSettings<vector_t>.number_particles),
        "Number of particles.")
      ("vmin",
        po::value<int>(&settings.vmin)->default_value(Simulation::DefaultSettings<vector_t>.vmin),
        "Minimum starting velocity.")
      ("vmax",
        po::value<int>(&settings.vmax)->default_value(Simulation::DefaultSettings<vector_t>.vmax),
        "Maximum starting velocity.")
      ("vall",
        po::value<int>(),
        "Start all particles at this velocity. Overrides --vmin and --vmax.")
      ("mass-min",
        po::value<float>(&settings.mass_min)->default_value(Simulation::DefaultSettings<vector_t>.mass_min),
        "Minimum mass allowed for a particle")
      ("mass-max",
        po::value<float>(&settings.mass_max)->default_value(Simulation::DefaultSettings<vector_t>.mass_max),
        "Maximum mass allowed for a particle")
      ("angle",
        po::value<float>(&settings.angle),
        "Start all particles travelling at this angle (in degrees).")
      ("radius-min",
        po::value<float>(&settings.radius_min)->default_value(Simulation::DefaultSettings<vector_t>.radius_min),
        "Particle radii, whole number.")
      ("radius-max",
        po::value<float>(&settings.radius_max)->default_value(Simulation::DefaultSettings<vector_t>.radius_max),
        "Particle radii, whole number.")
      ("color",
        po::value<std::vector<int>>()->multitoken(),
        "Color in R G B")
      ("color-range",
        po::value<std::vector<int>>()->multitoken(),
        "Requires --color, smooths one color to another.")
      ("display",
        po::bool_switch(&settings.display_mode)->default_value(Simulation::DefaultSettings<vector_t>.display_mode),
        "Zero velocity, just view colors.")
      ("delay",
        po::value(&settings.delay)->default_value(Simulation::DefaultSettings<vector_t>.delay),
        "Delay before we start running the simulation, in seconds.")
      ("no-full-screen",
        po::bool_switch()->default_value(false),
        "Disable default fullscreen.")
      ("no-gui",
        po::bool_switch(&settings.no_gui)->default_value(Simulation::DefaultSettings<vector_t>.no_gui),
        "Diable the GUI. Boring for users, great for debug.")
      ("trace",
        po::value<std::vector<size_t>>()->multitoken(),
        "Highlight these paricles and remove unrelated debug output.")
      ("extra-trace",
        po::bool_switch(&settings.extra_trace)->default_value(Simulation::DefaultSettings<vector_t>.extra_trace),
        "Print out full sim state after each iteration. This will be slow.")
      ("gravity",
        po::value<float>(&settings.gravity)->default_value(Simulation::DefaultSettings<vector_t>.gravity),
        "Add gravity to the simulation.")
      ("gravity-angle",
        po::value<float>(&settings.gravity_angle)->default_value(Simulation::DefaultSettings<vector_t>.gravity_angle),
        "Commit crimes against nature.")
      ("info",
        po::bool_switch(&settings.info)->default_value(Simulation::DefaultSettings<vector_t>.info),
        "Print out INFO level messages. System stats, etc.")
      ;

  // I normally detest exceptions, but this library throws one reasonably, no point guessing what
  // the user might've meant if the arguments aren't correct
  try {
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return Status::None;
    }

    if (!vm["no-full-screen"].defaulted()) {
      settings.screen_mode = Simulation::ScreenMode::DEFAULT;
    }

    if (vm.count("angle")) {
      settings.random_angle = false;
    }

    if (vm.count("vmax") && vm.count("vmin")) {
      if (vm["vmax"].as<int>() < vm["vmin"].as<int>()) {
        std::cout << "See --help, Must have vmax > vmin." << std::endl;
        return Status::Failure;
      }
    }

    if (vm.count("mass-max") && vm.count("mass-min")) {
      if (vm["mass-max"].as<float>() < vm["mass-min"].as<float>()) {
        std::cout << "See --help, Must have mass-max > mass-min." << std::endl;
        return Status::Failure;
      }
    }

    if (!vm["vmin"].defaulted() && vm["vmax"].defaulted() && !vm.count("vall")) {
      std::cout << "See --help, --vmin specified, without --vmax" << std::endl;
      return Status::Failure;
    }

    if (vm.count("vall")) {
      std::cout << "--vall specified, ignoring --vmax and --vmin" << std::endl;
      settings.vmax = settings.vmin = vm["vall"].as<int>();
    }

    // [0-255], [0-255], [0-255]
    auto validate_rgb = [](const std::vector<int>& v) {
      if (v.size() != 3 or *std::max_element(v.begin(), v.end()) > 255 or *std::min_element(v.begin(), v.end()) < 0) {
        std::cout << "Invalid rgb: " << std::endl;
        for (auto c : v) {
          std::cout << c << " ";
        }
        std::cout << std::endl;
        return false;
      }
      return true;
    };

    if (vm.count("color")) {
      settings.color = vm["color"].as<std::vector<int>>();
      if (!validate_rgb(settings.color)) {
        return Status::Failure;
      }
    }

    if (vm.count("color-range") && !vm.count("color")) {
      std::cout << "See --help, color-range requires color" << std::endl;
      return Status::Failure;
    } else if (vm.count("color-range")) {
      settings.color_range = vm["color-range"].as<std::vector<int>>();
      if (!validate_rgb(settings.color_range)) {
        return Status::Failure;
      }
    }

    if (vm.count("color-range")) {
      settings.color_range = vm["color-range"].as<std::vector<int>>();
    }

    if (vm.count("trace")) {
      settings.trace = vm["trace"].as<std::vector<size_t>>();
    }
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
    std::cout << desc << std::endl;
    return Status::Failure;
  }
  return Status::Success;
}

} // namespace Cli
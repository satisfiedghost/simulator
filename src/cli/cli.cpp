#include "cli.h"
#include <iostream>

bool parse_cli_args(int argc, char** argv, po::variables_map& vm, Simulation::SimSettings& settings) {
  po::options_description desc("Allowed options");
  desc.add_options()
      ("help", "Display this help message.")
      ("pcount", po::value<size_t>(&settings.number_particles)->default_value(Simulation::DefaultSettings.number_particles), "Number of particles.")
      ("vmax", po::value<int>(&settings.vmax)->default_value(Simulation::DefaultSettings.vmax), "Maximum starting velocity.")
      ("vmin", po::value<int>(&settings.vmin)->default_value(Simulation::DefaultSettings.vmin), "Minimum starting velocity.")
      ("vall", po::value<int>(), "Start all particles at this velocity. Overrides --vmin and --vmax.")
      ("angle", po::value<float>(&settings.angle), "Start all particles travelling at this angle (in degrees).")
      ("radius", po::value<size_t>(&settings.particle_radius)->default_value(Simulation::DefaultSettings.particle_radius), "Particle radii, whole number.")
      ("color", po::value<std::vector<int>>()->multitoken(), "Color in R G B")
      ("color-range", po::value<std::vector<int>>()->multitoken(), "Requires --color, smooths one color to another.")
      ("display", po::bool_switch(&settings.display_mode)->default_value(Simulation::DefaultSettings.display_mode), "Zero velocity, just view colors.")
      ("delay", po::value(&settings.delay)->default_value(Simulation::DefaultSettings.delay), "Delay before we start running the simulation, in seconds.")
      ("no-full-screen", po::bool_switch()->default_value(false), "Disable default fullscreen.")

  ;

  // I normally detest exceptions, but this library throws one reasonably, no point guessing what
  // the user might've meant if the arguments aren't correct
  try {
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return true;
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
        return false;
      }
    }

    if (!vm["vmin"].defaulted() && vm["vmax"].defaulted() && !vm.count("vall")) {
      std::cout << "See --help, --vmin specified, without --vmax" << std::endl;
      return false;
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
        return false;
      }
    }

    if (vm.count("color-range") && !vm.count("color")) {
      std::cout << "See --help, color-range requires color" << std::endl;
      return false;
    } else if (vm.count("color-range")) {
      settings.color_range = vm["color-range"].as<std::vector<int>>();
      if (!validate_rgb(settings.color_range)) {
        return false;
      }
    }

    if (vm.count("color-range")) {
      settings.color_range = vm["color-range"].as<std::vector<int>>();
    }
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
    std::cout << desc << std::endl;
    return false;
  }
  return true;
}
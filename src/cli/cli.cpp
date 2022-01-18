#include "cli.h"
#include "component.h"
#include "util/fixed_point.h"

#include <iostream>

// No constexpr std::string until C++20 :(
static constexpr char help_str[] = "help";
static constexpr char p_count_str[] = "p-count";
static constexpr char v_min_str[] = "v-min";
static constexpr char v_max_str[] = "v-max";
static constexpr char v_all_str[] = "v-all";
static constexpr char start_angle_str[] = "start-angle";
static constexpr char mass_min_str[] = "mass-min";
static constexpr char mass_max_str[] = "mass-max";
static constexpr char mass_all_str[] = "mass-all";
static constexpr char radius_min_str[] = "radius-min";
static constexpr char radius_max_str[] = "radius-max";
static constexpr char radius_all_str[] = "radius-all";
static constexpr char color_str[] = "color";
static constexpr char color_range_str[] = "color-range";
static constexpr char gravity_str[] = "gravity";
static constexpr char gravity_angle_str[] = "gravity-angle";
static constexpr char display_str[] = "display";
static constexpr char delay_str[] = "delay";
static constexpr char no_full_screen_str[] = "no-full-screen";
static constexpr char debug_no_gui_str[] = "debug-no-gui";
static constexpr char debug_trace_str[] = "debug-trace";
static constexpr char debug_extra_trace_str[] = "debug-extra-trace";
static constexpr char debug_info_str[] = "debug-info";
namespace Cli {

template <typename Vt>
Status parse_cli_args(int argc, char** argv, po::variables_map& vm, Simulation::SimSettings<Vt>& settings) {
  typedef Vt vector_t;
  po::options_description desc("Allowed options");
  desc.add_options()
      (help_str, "Display this help message.")
      (p_count_str,
        po::value<size_t>(&settings.number_particles)->default_value(Simulation::DefaultSettings<vector_t>.number_particles),
        "Total number of particles. Increasing this without decreasing radii may result in an expensive (i.e. long-running) computation "
        "to ensure particles do not overlap.")
      (v_min_str,
        po::value<int>(&settings.vmin)->default_value(Simulation::DefaultSettings<vector_t>.vmin),
        "Minimum starting velocity. Randomized within a default range if unspecified.")
      (v_max_str,
        po::value<int>(&settings.vmax)->default_value(Simulation::DefaultSettings<vector_t>.vmax),
        "Maximum starting velocity. Randomized within a default range if unspecified.")
      (v_all_str,
        po::value<int>(),
        "Start all particles at this velocity. Overrides --v-min and --v-max.")
      (start_angle_str,
        po::value<float>(&settings.angle),
        "Start all particles travelling at this angle (in degrees from the horizontal). Randomized if unspecified.")
      (mass_min_str,
        po::value<vector_t>(&settings.mass_min)->default_value(Simulation::DefaultSettings<vector_t>.mass_min),
        "Minimum mass allowed for a particle. Randomized within a default range if unspecified.")
      (mass_max_str,
        po::value<vector_t>(&settings.mass_max)->default_value(Simulation::DefaultSettings<vector_t>.mass_max),
        "Maximum mass allowed for a particle. Must be > mass_min Randomized within a default range if unspecified.")
      (mass_all_str,
        po::value<vector_t>(),
        "All particle's mass, overrides --mass_min and --mass_max.")
      (radius_min_str,
        po::value<vector_t>(&settings.radius_min)->default_value(Simulation::DefaultSettings<vector_t>.radius_min),
        "Minimum prticle radii. Randomized within a default range if unspecified.")
      (radius_max_str,
        po::value<vector_t>(&settings.radius_max)->default_value(Simulation::DefaultSettings<vector_t>.radius_max),
        "Maximum particle radii. Randomized within a default range if unspecified.")
      (radius_all_str,
        po::value<vector_t>(),
        "All particle's radii, overrides --radius_min and --radius_max")
      (color_str,
        po::value<std::vector<int>>()->multitoken(),
        "Color of all particles in RGB. e.g. specify --color 159 226 191 for a lovely seafoam green.")
      (color_range_str,
        po::value<std::vector<int>>()->multitoken(),
        "Requires --color, smooths one color to another. Simple linear smoothing, more color options coming in the future.")
      (gravity_str,
        po::value<vector_t>(&settings.gravity)->default_value(Simulation::DefaultSettings<vector_t>.gravity),
        "Add gravity to the simulation. This is the magnitude of the gravity-angle vector.")
      (gravity_angle_str,
        po::value<float>(&settings.gravity_angle)->default_value(Simulation::DefaultSettings<vector_t>.gravity_angle),
        "Commit crimes against nature. Direction of the gravity vector (in degrees from the horizontal).")
      (display_str,
        po::bool_switch(&settings.display_mode)->default_value(Simulation::DefaultSettings<vector_t>.display_mode),
        "Zero velocity, just view colors. Useful for checking initial conditions for e.g. making an interesting pattern.")
      (delay_str,
        po::value(&settings.delay)->default_value(Simulation::DefaultSettings<vector_t>.delay),
        "Delay before we start running the simulation, in seconds.")
      (no_full_screen_str,
        po::bool_switch()->default_value(false),
        "Disable default fullscreen.")
      (debug_no_gui_str,
        po::bool_switch(&settings.no_gui)->default_value(Simulation::DefaultSettings<vector_t>.no_gui),
        "Diable the GUI. Boring for users, great for debug.")
      (debug_trace_str,
        po::value<std::vector<size_t>>()->multitoken(),
        "Highlight these pariticles, color all others white, and remove debug output for other particles.")
      (debug_extra_trace_str,
        po::bool_switch(&settings.extra_trace)->default_value(Simulation::DefaultSettings<vector_t>.extra_trace),
        "Print out full sim state after each iteration. This will be very slow.")
      (debug_info_str,
        po::bool_switch(&settings.info)->default_value(Simulation::DefaultSettings<vector_t>.info),
        "Print out INFO level messages. System stats, etc.")
      ;

  // I normally detest exceptions, but this library throws one reasonably, no point guessing what
  // the user might've meant if the arguments aren't correct
  //
  // Side note this whole mess is in desperate need of cleanup but it's more fun to build features than the UX.
  try {
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    // Help!
    if (vm.count(help_str)) {
        std::cout << desc << std::endl;
        return Status::None;
    }

    // Radius settings
    if (vm.count(radius_min_str)) {
      if (vm[radius_min_str].as<vector_t>() < 0) {
        std::cout << "I have no idea what a particle would look like with a negative radius. Please let me know if you can show me a picture." << std::endl;
        return Status::Failure;
      }
    }

    if (vm.count(radius_max_str)) {
      if (vm[radius_min_str].as<vector_t>() < 0) {
        std::cout << "I have no idea what a particle would look like with a negative radius. Please let me know if you can show me a picture." << std::endl;
        return Status::Failure;
      }
    }

    if (vm.count(radius_min_str) && vm.count(radius_max_str)) {
      if (vm[radius_max_str].as<vector_t>() < vm[radius_min_str].as<vector_t>()) {
        std::cout << "See --help, Must have radius_max_str > radius_min_str." << std::endl;
        return Status::Failure;
      }
    }

    if (vm.count(radius_all_str)) {
      if (vm[radius_min_str].as<vector_t>() < 0) {
        std::cout << "I have no idea what a particle would look like with a negative radius. Please let me know if you can show me a picture." << std::endl;
        return Status::Failure;
      }
      settings.radius_min = settings.radius_max = vm[radius_all_str].as<vector_t>();
    }

    // Mass settings
    if (vm.count(mass_max_str) && vm.count(mass_min_str)) {
      if (vm[mass_max_str].as<vector_t>() < vm[mass_min_str].as<vector_t>()) {
        std::cout << "See --help, Must have mass_max_str > mass_min_str." << std::endl;
        return Status::Failure;
      }
    }

    if (vm.count(mass_min_str)) {
      if (vm[mass_min_str].as<vector_t>() < 0) {
        std::cout << "I don't support negative masses yet, although it's a cool idea." << std::endl;
        return Status::Failure;
      }
    }

    if (vm.count(mass_max_str)) {
      if (vm[mass_max_str].as<vector_t>() < 0) {
        std::cout << "I don't support negative masses yet, although it's a cool idea." << std::endl;
        return Status::Failure;
      }
    }

    if (vm.count(mass_all_str)) {
      if (vm[mass_all_str].as<vector_t>() < 0) {
        std::cout << "I don't support negative masses yet, although it's a cool idea." << std::endl;
        return Status::Failure;
      }
      settings.mass_min = settings.mass_max = vm[mass_all_str].as<vector_t>();
    }

    // Velocity settings
    if (vm.count(start_angle_str)) {
      settings.random_angle = false;
    }

    if (vm.count(v_all_str)) {
      std::cout << "--v-all specified, ignoring --v-max and --v-min" << std::endl;
      settings.vmax = settings.vmin = vm[v_all_str].as<int>();
    }

    if (vm.count(v_max_str) && vm.count(v_min_str)) {
      if (vm[v_max_str].as<int>() < vm[v_min_str].as<int>()) {
        std::cout << "See --help, Must have vmax > vmin." << std::endl;
        return Status::Failure;
      }
    }

    // Graphical settings
    if (!vm[no_full_screen_str].defaulted()) {
      settings.screen_mode = Simulation::ScreenMode::DEFAULT;
    }

    // [0-255], [0-255], [0-255]
    auto validate_rgb = [](const std::vector<int>& v) {
      if (v.size() != 3 or *std::max_element(v.begin(), v.end()) > 255 or *std::min_element(v.begin(), v.end()) < 0) {
        std::cout << "Invalid RGB: ";
        for (auto c : v) {
          std::cout << c << " ";
        }
        std::cout << std::endl;
        return false;
      }
      return true;
    };

    if (vm.count(color_str)) {
      settings.color = vm[color_str].as<std::vector<int>>();
      if (!validate_rgb(settings.color)) {
        return Status::Failure;
      }
    }

    if (vm.count(color_range_str) && !vm.count(color_str)) {
      std::cout << "See --help, color_str-range requires color_str" << std::endl;
      return Status::Failure;
    } else if (vm.count(color_range_str)) {
      settings.color_range = vm[color_range_str].as<std::vector<int>>();
      if (!validate_rgb(settings.color_range)) {
        return Status::Failure;
      }
    }

    if (vm.count(color_range_str)) {
      settings.color_range = vm[color_range_str].as<std::vector<int>>();
    }

    // Debug settings
    if (vm.count(debug_trace_str)) {
      settings.trace = vm[debug_trace_str].as<std::vector<size_t>>();
    }
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
    std::cout << desc << std::endl;
    return Status::Failure;
  }
  return Status::Success;
}

template
Status parse_cli_args(int argc, char** argv, po::variables_map& vm,
  Simulation::SimSettings<float>& settings);

template
Status parse_cli_args(int argc, char** argv, po::variables_map& vm,
  Simulation::SimSettings<double>& settings);

template
Status parse_cli_args(int argc, char** argv, po::variables_map& vm,
  Simulation::SimSettings<Util::FixedPoint>& settings);

} // namespace Cli
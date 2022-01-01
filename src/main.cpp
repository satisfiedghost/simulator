#include "window.h"
#include <thread>
#include <time.h>
#include <random>
#include <cmath>
#include <chrono>

#include <boost/program_options.hpp>

constexpr size_t x_width = 1600;
constexpr size_t y_width = 800;
constexpr size_t z_width = 1000;

namespace po = boost::program_options;
namespace chrono = std::chrono;

void sim_runner(Simulation::SimulationContext& sim, float delay = 0) {
  if (delay > 0) {
    auto start = chrono::steady_clock::now();
    chrono::duration<float> elapsed;
    do {
      auto now = chrono::steady_clock::now();
      elapsed = now - start;
    } while (elapsed.count() < delay);
  }
  while(true) {
    sim.run();
  }
}

int main(int argc, char** argv) {
  size_t number_particles;
  int vmax = 0;
  int vmin = 0;
  float angle = 0;
  bool random_angle = true;
  std::vector<int> color;
  std::vector<int> color_range;
  bool display_mode = false;
  float delay = 0;

  po::options_description desc("Allowed options");
  desc.add_options()
      ("help", "Display this help message.")
      ("pcount", po::value<size_t>(&number_particles)->default_value(400), "Number of particles.")
      ("vmax", po::value<int>(&vmax)->default_value(250), "Maximum starting velocity.")
      ("vmin", po::value<int>(&vmin)->default_value(0), "Minimum starting velocity.")
      ("angle", po::value<float>(&angle), "Start all particles travelling at this angle (in degrees).")
      ("color", po::value<std::vector<int>>()->multitoken(), "Color in R G B")
      ("color-range", po::value<std::vector<int>>()->multitoken(), "Requires --color, smooths one color to another.")
      ("display", po::bool_switch(&display_mode)->default_value(false), "Zero velocity, just view colors.")
      ("delay", po::value(&delay)->default_value(0), "Delay before we start running the simulation, in seconds.")
  ;

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
    std::cout << desc << std::endl;
    return 1;
  }

  if (vm.count("help")) {
      std::cout << desc << std::endl;
      return 1;
  }

  if (vm.count("angle")) {
    random_angle = false;
  }

  if (vm.count("vmax")) {
    if (vm["vmax"].as<int>() == 0) {
      display_mode = true;
    }
  }

  auto validate_rgb = [](const std::vector<int>& v) {
    if (v.size() != 3 or *std::max_element(v.begin(), v.end()) > 255) {
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
    color = vm["color"].as<std::vector<int>>();
    if (!validate_rgb(color)) {
      return 1;
    }
  }

  if (vm.count("color-range") && !vm.count("color")) {
    std::cout << "See --help, color-range requires color" << std::endl;
    return 1;
  } else if (vm.count("color-range")) {
    color_range = vm["color-range"].as<std::vector<int>>();
    if (!validate_rgb(color_range)) {
      return 1;
    }
  }

  if (vm.count("color-range")) {
    color_range = vm["color-range"].as<std::vector<int>>();
  }

  auto deg_to_rad = [](float angle) {
    return angle * M_PI / 180;
  };


  Simulation::SimulationContext sim;
  sim.set_boundaries(x_width, y_width, z_width);

  const size_t grid = std::floor(std::sqrt(number_particles));

  size_t x_spacing = (x_width - 100) / grid;
  size_t y_spacing = (y_width - 100) / grid;

  std::random_device rd;  // Will be used to obtain a seed for the
  std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<int> vel_dist(0, vmax * 2);

  for (size_t i = 0; i < number_particles; i++) {
    auto vx = 0, vy = 0;
    // vectorwise adds up to vmax
    if (!display_mode) {
      if (random_angle) {
        vy = vel_dist(gen) % vmax - (vmax / 2);
        vx = std::sqrt(std::pow(vmax, 2) - std::pow(vy, 2));
        if (static_cast<int>(vx) % 2) {
          vx *= -1;
        }
      } else {
        auto rads = deg_to_rad(angle);
        auto v_mag = std::max(vmin, vel_dist(gen) / 2);
        vx = std::cos(rads) * v_mag;
        vy = std::sin(rads) * v_mag;
      }
    }

    Simulation::Vector<float> velocity{static_cast<float>(vx), static_cast<float>(vy), 0.f};

    if (!display_mode) {
      if (velocity.magnitude < vmin) {
        velocity = velocity.collinear_vector(vmin);
      } else if (velocity.magnitude > vmax) {
        velocity = velocity.collinear_vector(vmax);
      }
    }

    int px = (-(x_width / 2) + 100) + (i % grid) * x_spacing;
    int py = (y_width / 2) - 100 - (i / grid) * y_spacing;

    Simulation::Vector<float> p{static_cast<float>(px), static_cast<float>(py), 0.f};

    sim.add_particle(velocity, p);
  }

  std::thread window(Graphics::SimulationWindow, std::ref(sim), color, color_range);
  std::thread sim_thread(sim_runner, std::ref(sim), delay);

  window.join();
  sim_thread.join();

  return 0;
}
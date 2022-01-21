#include "cli.h"
#include "demo/demo.h"
#include "window.h"

// change to run the system with different underlying types!
typedef Component::Vector<Util::FixedPoint> sim_t;
//typedef Component::Vector<double> sim_t;
//typedef Component::Vector<float> sim_t;

int main(int argc, char** argv) {
  Simulation::SimulationContext<sim_t> sim;
  Simulation::SimSettings<typename sim_t::vector_t> settings = Simulation::DefaultSettings<typename sim_t::vector_t>;

  po::variables_map vm;
  Status s = Cli::parse_cli_args<typename sim_t::vector_t>(argc, argv, vm, settings);

  switch(s) {
    case Status::None:
      return 0;
    case Status::Failure:
      std::cout << "Failed to parse arguments, terminating." << std::endl;
      return 1;
    default:
      break;
  }

  Demo::set_initial_conditions<sim_t>(sim, settings);
  Simulation::PhysicsContext<sim_t> physics_context(settings);

  sim.set_physics_context(physics_context);

  std::thread window_thread;
  std::thread sim_thread(Simulation::SimulationContextThread<sim_t>, std::ref(sim), settings);

  if (settings.no_gui) {
    // start the sim thread only, and run until it's done
    sim_thread.join();
  } else {
    // start both threads, and run until the window thread is closed
    window_thread = std::thread(Graphics::SimulationWindowThread<sim_t>, std::ref(sim), settings);
    window_thread.join();
    sim_thread.detach();
  }

  std::cout << "Goodbye!" << std::endl;
  return 0;
}
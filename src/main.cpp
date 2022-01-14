#include "cli.h"
#include "demo/demo.h"
#include "window.h"

typedef Component::Vector<Util::FixedPoint> sim_t;

int main(int argc, char** argv) {
  Simulation::SimulationContext<sim_t> sim;
  Simulation::SimSettings<typename sim_t::vector_t> settings = Simulation::DefaultSettings<typename sim_t::vector_t>;

  po::variables_map vm;
  Status s = Cli::parse_cli_args<sim_t>(argc, argv, vm, settings);

  switch(s) {
    case Status::None:
      return 0;
    case Status::Failure:
      std::cout << "Failed to parse arguments, terminating." << std::endl;
      return 1;
    default:
      break;
  }

  Demo::set_initial_conditions(sim, settings);
  Simulation::PhysicsContext<sim_t> physics_context(settings);

  sim.set_physics_context(physics_context);

  std::thread window_thread;

  if (!settings.no_gui) {
    window_thread = std::thread(Graphics::SimulationWindowThread<sim_t>, std::ref(sim), settings);
  }

  std::thread sim_thread(Simulation::SimulationContextThread<sim_t>, std::ref(sim), settings);

  if (!settings.no_gui) {
    window_thread.join();
  }
  sim_thread.join();

  std::cout << "Goodbye!" << std::endl;
  return 0;
}
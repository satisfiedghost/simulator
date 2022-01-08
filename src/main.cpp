#include "cli.h"
#include "demo/demo.tpp"
#include "window.h"

int main(int argc, char** argv) {
  Simulation::SimulationContext<double> sim;
  Simulation::SimSettings settings = Simulation::DefaultSettings;

  po::variables_map vm;
  Status s = Cli::parse_cli_args(argc, argv, vm, settings);

  switch(s) {
    case Status::None:
      return 0;
    case Status::Failure:
      std::cout << "Failed to parse arguments, terminating." << std::endl;
      return 1;
    default:
      break;
  }

  set_initial_conditions(sim, settings);
  Simulation::PhysicsContext<double> physics_context(settings);

  sim.set_physics_context(physics_context);

  std::thread window_thread;

  if (!settings.no_gui) {
    window_thread = std::thread(Graphics::SimulationWindowThread<double>, std::ref(sim), settings);
  }

  std::thread sim_thread(Simulation::SimulationContextThread<double>, std::ref(sim), settings);

  if (!settings.no_gui) {
    window_thread.join();
  }
  sim_thread.join();

  std::cout << "Goodbye!" << std::endl;
  return 0;
}
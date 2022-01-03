#include "cli.h"
#include "demo/demo.tpp"
#include "window.h"

int main(int argc, char** argv) {
  Simulation::SimulationContext<float> sim;
  Simulation::SimSettings settings = Simulation::DefaultSettings;

  po::variables_map vm;
  if (!Cli::parse_cli_args(argc, argv, vm, settings)) {
    // bye!
    std::cout << "Failed to parse arguments, terminating." << std::endl;
    return 1;
  } else if (vm.count("help")) {
    return 0;
  }

  set_initial_conditions(sim, settings);

  std::thread window_thread(Graphics::SimulationWindowThread<float>, std::ref(sim), settings);
  std::thread sim_thread(Simulation::SimulationContextThread<float>, std::ref(sim), settings);

  window_thread.join();
  sim_thread.join();

  std::cout << "Goodbye!" << std::endl;
  return 0;
}
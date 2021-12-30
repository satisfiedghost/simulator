#include "window.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <time.h>

void Graphics::SimulationWindow(const Simulation::SimulationContext& sim) {
  // create the window
  sf::RenderWindow window(sf::VideoMode(1600, 900), "My window");

  std::vector<sf::CircleShape> draw_particles;

  const auto& sim_particles = sim.get_particles();
  constexpr float RADIUS = 10.f;

  constexpr float X_OFFSET = 800 - RADIUS;
  constexpr float Y_OFFSET = 450 - RADIUS;

  for (size_t i = 0; i < sim_particles.size(); i++) {
    sf::CircleShape tshape(RADIUS);
    draw_particles.push_back(tshape);
  }

  srand (time(NULL));
  for (auto & p : draw_particles) {
    auto r = rand() % 256;
    auto g = rand() % 256;
    auto b = rand() % 256;
    p.setFillColor(sf::Color(r, g, b));
  }

  // get the clock now
  sf::Clock clock;


  sf::Time last_draw = clock.getElapsedTime();
  // run the program as long as the window is open
  while (window.isOpen())
  {
      const auto& sim_particles = sim.get_particles();
      sf::Time now = clock.getElapsedTime();
      // check all the window's events that were triggered since the last iteration of the loop
      sf::Event event;
      while (window.pollEvent(event))
      {
          // "close requested" event: we close the window
          if (event.type == sf::Event::Closed)
              window.close();
      }

      //if (now - last_draw > sf::microseconds(100)) {
        // clear the window with black color
        window.clear(sf::Color::Black);

        // draw everything here...
        size_t idx = 0;
        for (const auto& p : sim_particles) {
          auto pos = p.get_position();
          draw_particles[idx].setPosition(pos.one() + X_OFFSET, -pos.two() + Y_OFFSET - 50);
          window.draw(draw_particles[idx]);
          idx++;
        }

        // end the current frame
        window.display();
        last_draw = now;
      //}
  }
}
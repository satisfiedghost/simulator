#include "window.h"
#include <SFML/Graphics.hpp>
#include <vector>

void Graphics::SimulationWindow(const Simulation::SimulationContext& sim) {
  (void)sim;
  // create the window
  sf::RenderWindow window(sf::VideoMode(1600, 900), "My window");

  std::vector<sf::CircleShape> draw_particles;

  const auto& sim_particles = sim.get_particles();

  for (const auto& p : sim_particles) {
    (void)p;
    sf::CircleShape tshape(10.f);
    draw_particles.push_back(tshape);
  }

  for (auto& p : draw_particles) {
    p.setOrigin(-800, -450);
  }

  draw_particles[0].setFillColor(sf::Color(150, 50, 30));
  draw_particles[1].setFillColor(sf::Color(252, 186, 3));
  draw_particles[2].setFillColor(sf::Color(111, 218, 232));
  draw_particles[3].setFillColor(sf::Color(61, 65, 145));
  draw_particles[4].setFillColor(sf::Color(0, 250, 0));

  // get the clock now
  sf::Clock clock;


  sf::Time last_draw = clock.getElapsedTime();
  // run the program as long as the window is open
  while (window.isOpen())
  {
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
          draw_particles[idx].setPosition(pos.one(), pos.two());
          window.draw(draw_particles[idx]);
          idx++;
        }

        // end the current frame
        window.display();
        last_draw = now;
      //}
  }
}
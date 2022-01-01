#include "window.h"
#include <SFML/Graphics.hpp>
#include <time.h>

void Graphics::SimulationWindow(const Simulation::SimulationContext& sim,
                                std::vector<int> color,
                                std::vector<int> color_range) {
  bool user_color = false;
  bool user_color_range = false;
  if (color.size() > 0) {
    user_color = true;
  }
  if (color_range.size() > 0) {
    user_color_range = true;
  }


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

  if (!user_color) {
    srand (time(NULL));
    for (auto& p : draw_particles) {
      auto r = rand() % 256;
      auto g = rand() % 256;
      auto b = rand() % 256;
      p.setFillColor(sf::Color(r, g, b));
    }
  } else if (!user_color_range) {
    for (auto& p : draw_particles) {
      p.setFillColor(sf::Color(color[0], color[1], color[2]));
    }
  } else {
    float r_step = static_cast<float>(color_range[0] - color[0]) / static_cast<float>(draw_particles.size());
    float g_step = static_cast<float>(color_range[1] - color[1]) / static_cast<float>(draw_particles.size());
    float b_step = static_cast<float>(color_range[2] - color[2]) / static_cast<float>(draw_particles.size());

    // float to avoid integer rounding when stepping, but ultimately we need to stop on some int rgb value...
    float red = color[0];
    float grn = color[1];
    float blu = color[2];

    for (auto& p : draw_particles) {
      p.setFillColor(sf::Color(static_cast<int>(red), static_cast<int>(grn), static_cast<int>(blu)));
      red += r_step;
      grn += g_step;
      blu += b_step;
    }

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
  }
}
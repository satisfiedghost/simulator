#include "window.h"
#include <SFML/Graphics.hpp>
#include <time.h>
#include <memory>
#include "sim_settings.h"

void Graphics::SimulationWindowThread(const Simulation::SimulationContext& sim, Simulation::SimSettings settings) {
  bool user_color = false;
  bool user_color_range = false;
  auto color = settings.color;
  auto color_range = settings.color_range;

  if (color.size() > 0) {
    user_color = true;
  }
  if (color_range.size() > 0) {
    user_color_range = true;
  }

  // create the window
  auto desktop_mode = sf::VideoMode::getDesktopMode();

  // defer initialization becuase copy ctor is delete
  std::unique_ptr<sf::RenderWindow> window;
  switch(settings.screen_mode) {
    case Simulation::ScreenMode::FULLSCREEN:
      window = std::make_unique<sf::RenderWindow>(desktop_mode, "Particles!", sf::Style::Fullscreen);
    break;
    case Simulation::ScreenMode::DEFAULT:
      window = std::make_unique<sf::RenderWindow>(desktop_mode, "Particles!", sf::Style::Default);
    break;
  }

  std::vector<sf::CircleShape> draw_particles;

  const auto& sim_particles = sim.get_particles();
  const float RADIUS = settings.particle_radius;

  const float X_OFFSET = (desktop_mode.width / 2) - RADIUS;
  const float Y_OFFSET = (desktop_mode.height / 2) - RADIUS;

  for (size_t i = 0; i < sim_particles.size(); i++) {
    sf::CircleShape tshape(RADIUS);
    draw_particles.push_back(tshape);
  }

  if (!user_color) {
    srand (time(NULL));
    for (auto& p : draw_particles) {
      int rgb[3] = {rand() % 256, rand() % 256, rand() % 256};

      // don't draw invisible particles
      if (std::max(rgb[0], std::max(rgb[1], rgb[2])) < 10) {
        auto boost_color = rand() % 3;
        rgb[boost_color] += std::max(rand() % 256, 80);
      }
      p.setFillColor(sf::Color(rgb[0], rgb[1], rgb[2]));
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
  while (window->isOpen())
  {
      const auto& sim_particles = sim.get_particles();
      sf::Time now = clock.getElapsedTime();
      // check all the window's events that were triggered since the last iteration of the loop
      sf::Event event;
      while (window->pollEvent(event))
      {
          // "close requested" event: we close the window
          if (event.type == sf::Event::Closed)
              window->close();
      }

      window->clear(sf::Color::Black);

      // draw everything here...
      size_t idx = 0;
      for (const auto& p : sim_particles) {
        auto pos = p.get_position();
        draw_particles[idx].setPosition(pos.one() + X_OFFSET, -pos.two() + Y_OFFSET);
        window->draw(draw_particles[idx]);
        idx++;
      }

      // end the current frame
      window->display();
      last_draw = now;
  }
}

std::tuple<size_t, size_t, size_t> Graphics::get_window_size() {
  auto desktop_mode = sf::VideoMode::getDesktopMode();
  return std::make_tuple(desktop_mode.width, desktop_mode.height, Simulation::DefaultSettings.z_width);
}
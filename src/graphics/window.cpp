#include "sim_settings.h"
#include "window.h"

#include <SFML/Graphics.hpp>

#include <memory>
#include <time.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"

namespace Graphics {

struct DrawParticle {
  sf::CircleShape particle;
  float x_draw_offset;
  float y_draw_offset;

  operator sf::CircleShape&() {return particle;}

  void set_position(float x, float y) {
    particle.setPosition(x + x_draw_offset, y + y_draw_offset);
  }
};

template <typename T>
void SimulationWindowThread(const Simulation::SimulationContext<T>& sim, Simulation::SimSettings settings) {
  std::vector<DrawParticle> draw_particles;

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


  for (const auto& p : sim.get_particles()) {
    sf::CircleShape tshape(p.get_radius());
    draw_particles.push_back({tshape,
                              (desktop_mode.width / 2) - static_cast<float>(p.get_radius()),
                              (desktop_mode.height / 2) - static_cast<float>(p.get_radius())});
  }

  if (!user_color) {
    srand (time(NULL));
    for (sf::CircleShape& p : draw_particles) {
      int rgb[3] = {rand() % 256, rand() % 256, rand() % 256};

      // don't draw invisible particles
      if (std::max(rgb[0], std::max(rgb[1], rgb[2])) < 10) {
        auto boost_color = rand() % 3;
        rgb[boost_color] += std::max(rand() % 256, 80);
      }
      p.setFillColor(sf::Color(rgb[0], rgb[1], rgb[2]));
    }
  } else if (!user_color_range) {
    for (sf::CircleShape& p : draw_particles) {
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

    for (sf::CircleShape& p : draw_particles) {
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
      while (window->pollEvent(event)) {
          // "close requested" event: we close the window
          if (event.type == sf::Event::Closed)
              window->close();
      }

      window->clear(sf::Color::Black);

      // draw everything here...
      size_t idx = 0;
      for (const auto& p : sim_particles) {
        auto pos = p.get_position();

        draw_particles[idx].set_position(pos.one(), -pos.two());
        window->draw(draw_particles[idx]);
        idx++;
      }

      // end the current frame
      window->display();
      last_draw = now;
  }
}

std::tuple<size_t, size_t, size_t> get_window_size() {
  auto desktop_mode = sf::VideoMode::getDesktopMode();
  return std::make_tuple(desktop_mode.width, desktop_mode.height, Simulation::DefaultSettings.z_width);
}

template void SimulationWindowThread(const Simulation::SimulationContext<float>& sim, Simulation::SimSettings settings);
template void SimulationWindowThread(const Simulation::SimulationContext<double>& sim, Simulation::SimSettings settings);

} // namespace Graphics
#pragma GCC diagnostic pop
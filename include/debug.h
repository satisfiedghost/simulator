#pragma once
// Store debug messages here as macros to avoid cluttering source code with blocks of cout <<
// macros will always be designed in such a way that DEBUG_MSG(X); results in intended behavior (e.g. semicolons are fair game)
#define NOTE_REPLAY_HEADER \
  if (in_replay_mode) { \
    std::cout << "> (Note: The following event is occurring as part of a requested replay.)" << std::endl; \
  } \

#define NOTE_REPLAY_FOOTER \
  if (in_replay_mode) { \
    std::cout << "> (Note: The preceeding event has occurred as part of a requested replay.)" << std::endl; \
  } \

#define COLLISION_DATA_HEADER \
  std::cout << "*******************Collision detected!*******************" << std::endl; \
  std::cout << "*********************************************************" << std::endl; \
  NOTE_REPLAY_HEADER \

#define COLLISION_DATA_FOOTER \
    NOTE_REPLAY_FOOTER \
    std::cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << std::endl; \
    std::cout << "$$$$$$$$$$$$$$$$$$End Collision Data$$$$$$$$$$$$$$$$$$" << std::endl << std::endl; \

#define BOUNCE_DATA_HEADER \
    std::cout << "*******************Bounce detected!*******************" << std::endl; \
    std::cout << "******************************************************" << std::endl; \
    NOTE_REPLAY_HEADER \

#define BOUNCE_DATA_FOOTER \
    NOTE_REPLAY_FOOTER \
    std::cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << std::endl; \
    std::cout << "$$$$$$$$$$$$$$$$$$End Bounce Data$$$$$$$$$$$$$$$$$$" << std::endl << std::endl; \

#define SYSTEM_REPORT_HEADER \
    std::cout << "***************System Report (Post Run)***************" << std::endl; \
    std::cout << "******************************************************" << std::endl; \

#define SYSTEM_REPORT_FOOTER \
    std::cout << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$" << std::endl; \
    std::cout << "$$$$$$$$$$$$$$$End System Report (Post Run)$$$$$$$$$$$$$$$" << std::endl << std::endl; \

#define IMPOSSIBLE_COLLISION_WARNING \
    if (Simulation::trace_present(m_settings.get().trace, a.uid.get()) or \
        Simulation::trace_present(m_settings.get().trace, b.uid.get())) { \
        std::cout << "!!!!!!!!!!!!!!!!!!Detected impossible collision!!!!!!!!!!!!!!!!!!!" << std::endl << std::endl; \
        std::cout << "> Particle Status (Would Have Been): " << std::endl << std::endl; \
        std::cout << "Particle A (Would Have Been): " << std::endl << a << std::endl; \
        std::cout << "----------------------------------------------------" << std::endl; \
        std::cout << "Particle B (Would Have Been): " << std::endl << b << std::endl << std::endl; \
        std::cout << "Delta KE (Would Have Been): " << k_delta << std::endl; \
        std::cout << "Total KE (Would Have Been): " << ka_after + kb_after << std::endl << std::endl; \
    }

#define COLLISION_DETECTED \
  if (Simulation::trace_present(m_settings.get().trace, a.uid.get()) or \
      Simulation::trace_present(m_settings.get().trace, b.uid.get())) { \
    COLLISION_DATA_HEADER \
    std::cout << "On Step: " << m_outer_sim->get_step() << std::endl; \
    auto elapsed_time = m_outer_sim->get_elapsed_time_us().count(); \
    std::cout << "Elapsed Time (Real): " << elapsed_time << "us " << "| (" \
      << static_cast<float>(elapsed_time) / static_cast<float>(1e6) << "s)" << std::endl; \
    std::cout << "Distance: " << dist << std::endl; \
    std::cout << "V Delta: " << v_delta_before << std::endl << std::endl; \
    std::cout << "> Particle Status Pre-Collision: " << std::endl << std::endl; \
    std::cout << "Particle A (Pre): " << std::endl << a << std::endl; \
    std::cout << "----------------------------------------------------" << std::endl; \
    std::cout << "Particle B (Pre): " << std::endl << b << std::endl << std::endl; \
    std::cout << "Total KE (Pre): " << ka_before + kb_before << std::endl; \
    std::cout << "Impulse unit vector: " << impulse_unit_vector << std::endl; \
    std::cout << "Impulse vector: " << impulse_vector << std::endl << std::endl;; \
  } \

#define UNABLE_TO_CORRECT_COLLISION \
    std::cout << "Final Result: Unable To Correct Collision." << std::endl; \
    COLLISION_DATA_FOOTER \

// don't print this out on our internal recursive call
// we also need to recalculate KE since it might've changed as a result of correction
#define POST_COLLISION_REPORT \
  const auto k_delta_final = abs((ka_before + kb_before) - (a.get_kinetic_energy() + b.get_kinetic_energy())); \
  const auto ka_after_final = a.get_kinetic_energy(); \
  const auto kb_after_final = b.get_kinetic_energy(); \
  if (Simulation::trace_present(m_settings.get().trace, a.uid.get()) or \
      Simulation::trace_present(m_settings.get().trace, b.uid.get())) { \
    std::cout << "> Particle Status Post-Collision: " << std::endl << std::endl; \
    std::cout << "Particle A (Post): " << std::endl << a << std::endl; \
    std::cout << "----------------------------------------------------" << std::endl; \
    std::cout << "Particle B (Post): " << std::endl << b << std::endl << std::endl; \
    std::cout << "Delta KE: " << k_delta_final << std::endl; \
    std::cout << "Total KE (Post): " << ka_after_final + kb_after_final << std::endl; \
    COLLISION_DATA_FOOTER \
  } \

#define CORRECTION_ATTEMPT_FAILED \
    std::cout << "Unable To Correct Collision. On Attempt: " \
      << attempt << " of " << scales.size() << "." << std::endl << std::endl; \

#define CORRECTION_ATTEMPT_SUCCESS \
    std::cout << "Successfully corrected collision! On Attempt: " \
      << attempt << " of " << scales.size() << "." << std::endl << std::endl; \

#define CORRECTION_ATTEMPT_REPORT \
    std::cout << "Attempting a Correction. This will be attempt: " << attempt << " of " << scales.size() << "." << std::endl; \
    std::cout << "Replaying sim with time resolution: " << std::get<0>(scale).count() << "us" << std::endl << std::endl; \

#define BOUNCE_DETECTION \
  if (Simulation::trace_present(m_settings.get().trace, p.uid.get())) { \
    BOUNCE_DATA_HEADER \
    std::cout << "On Step: " << m_outer_sim->get_step() << std::endl; \
    auto elapsed_time = m_outer_sim->get_elapsed_time_us().count(); \
    std::cout << "Elapsed Time (Real): " << elapsed_time << "us " << "| (" \
      << static_cast<float>(elapsed_time) / static_cast<float>(1e6) << "s)" << std::endl; \
    std::cout << "Wall: " << wall << std::endl; \
    std::cout << "Particle (Post):" << std::endl << p << std::endl; \
    BOUNCE_DATA_FOOTER \
  } \

#define SYSTEM_STATS \
  std::cout << "Bounces: " << m_bounce_count << std::endl; \
  std::cout << "Collisions : " << m_collision_count << std::endl; \
  std::cout << "Corrections: " << m_correction_count << " (" << static_cast<float>(m_correction_count) / \
    static_cast<float>(m_correction_count + m_inconsistent_count) * 100 << "% of impossible collisions)" << std::endl; \
  std::cout << "Inconsistencies: " << m_inconsistent_count << " (" << static_cast<float>(m_inconsistent_count) / \
    static_cast<float>(m_collision_count) * 100 << "%)" << std::endl << std::endl; \

#define SYSTEM_REPORT \
  { \
  vector_t total_energy = 0; \
  static size_t last_frame = 0; \
  if (m_step - last_frame > TICKS_PER_SECOND or m_settings.get().extra_trace) { \
    last_frame = m_step; \
    std::cout << "***************System Report (Post Run)***************" << std::endl; \
    std::cout << "******************************************************" << std::endl; \
    std::cout << "On Step: " << m_step << std::endl; \
    auto elapsed_time = get_elapsed_time_us().count(); \
    std::cout << "Elapsed Time (Real): " << elapsed_time << "us " << "| (" \
      << static_cast<float>(elapsed_time) / static_cast<float>(1e6) << "s)" << std::endl; \
    for (const auto& p : *particles) { \
      if (Simulation::trace_present(m_settings.get().trace, p.uid.get())) { \
        std::cout << p << std::endl; \
        std::cout << "----------------------------------------------------" << std::endl; \
        total_energy += p.get_kinetic_energy(); \
      } \
    } \
    std::cout << "Total System KE: " << total_energy << std::endl; \
    SYSTEM_STATS \
    SYSTEM_REPORT_FOOTER \
  } \
  } \

#define COLLISION_CORRECTION_REPORT \
  std::cout << "Attempting to correct collision..." << std::endl; \
  std::cout << "The current system time resolution is " << SIM_RESOLUTION_US.count() << "us." << std::endl; \
  std::cout << "> Particle Status (1 Frame Ago):" << std::endl << std::endl; \
  std::cout << "Particle A (1 Frame Ago): " << std::endl << a_old << std::endl; \
  std::cout << "----------------------------------------------------" << std::endl; \
  std::cout << "Particle B (1 Frame Ago): " << std::endl << b_old << std::endl << std::endl; \

#ifdef DEBUG
#define DEBUG_MSG(X) X
#else
#define DEBUG_MSG(X) do {} while(0)
#endif

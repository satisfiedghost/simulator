#pragma once
// Informational messages are always printed. These should be lightweight and shown infrequently.
#include "debug.h"

#define SYSTEM_STATUS \
{ \
static size_t last_frame = 0; \
if (m_settings.get().info and (m_step - last_frame) > TICKS_PER_SECOND * 5) { \
  last_frame = m_step; \
  SYSTEM_STATS \
  std::cout << std::endl; \
} \
} \

#define INFO_MSG(X) X
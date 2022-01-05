#pragma once
#include "util/status.h"

#include <algorithm>
#include <array>
#include <iostream>
#include <memory>
#include <thread>
#include <type_traits>

// how many, and what Elemment tyep
namespace Util {
/**
 *  A ring buffer with semantics to allow for delayed copying.
 *  Convenient to allow us to read/write to the same memory in high-criticality
 *  sections and defer the copy operations for later.
 *
 *  C is a container which must implement the named requirements of SequenceContainer
 *  https://en.cppreference.com/w/cpp/named_req/SequenceContainer
 *
 *  E is the element type
 *
 *  T is the size
 *
 *  Technically only loosely thread safe. put() is essesntially atomic as long as there
 *  is only 1 consumer of the ring buffer
 *
 **/
template<typename C, typename E, std::size_t T>
class ThreadedRingBuffer;

template<typename C, typename E, size_t T>
void ring_thread(ThreadedRingBuffer<C, E, T>&);

template<typename C, typename E, std::size_t T>
class ThreadedRingBuffer {
static_assert(std::is_same<typename C::value_type, E>::value, "Container must have same elements as element type!");

public:
  ThreadedRingBuffer()
        : m_current_idx(0)
        , m_buffer(std::make_shared<C>())
        , needs_commit(false)
        , has_latest(false)
        {}

  // write-through to the working buffer
  void push_back(E ele) {
    m_buffer->push_back(ele);
    m_items[m_current_idx].push_back(ele);
  }

  // tell the ringbuffer you're ready to commit
  void put() {
    needs_commit = true;
  }

  // get read-only access to the latest item in a good state
  const C& latest() const {
    return m_items[m_current_idx];
  }

  static void start(ThreadedRingBuffer& trb) {
    std::thread th = std::thread(ring_thread, trb);
    th.detach();
  }

  // gets a writable container
  // Status::Success if the buffer is valid and ready
  // Status::NotReady if we're busy
  // this buffer can be freely written to until put() is called,
  //   at which point callers must successfully call get_writeable() again
  Status get_writeable(std::shared_ptr<C>& ptr) {
    if (needs_commit) {
      return Status::NotReady;
    } else {
      ptr = m_buffer;
    }
    return Status::Success;
  }

  size_t size() { return T; }

  size_t get_idx() { return m_current_idx; }

  template<typename Cc, typename Ec, size_t Tc>
  friend void ring_thread(ThreadedRingBuffer<Cc, Ec, Tc>&);

private:
  size_t get_next_idx() {
    return (m_current_idx + 1) % T;
  }

  size_t get_last_idx() {
    return (m_current_idx == 0) ? T : m_current_idx - 1;
  }


  size_t m_current_idx;
  std::array<C, T> m_items;
  std::shared_ptr<C> m_buffer;
  volatile bool needs_commit;
  bool has_latest;
};

template<typename C, typename E, size_t T>
std::ostream& operator<<(std::ostream& os, const ThreadedRingBuffer<C, E, T>& buffer) {
  os << "RingBuffer @ " << &buffer << std::endl;
  std::cout << "Size: " << buffer.size() << " Current Idx: " << buffer.get_idx();
  return os;
}

template<typename C, typename E, size_t T>
void ring_thread(ThreadedRingBuffer<C, E, T>& trb) {
  while(true) {
    while (!trb.needs_commit) {
      // try to be a good citizen
      std::this_thread::sleep_for(std::chrono::microseconds(50));
    }

    // write to the next idx of the buffer, which we will then mark as ready
    auto& from = trb.m_buffer;
    auto& to = trb.m_items[trb.get_next_idx()];

    to = (*from);

    trb.m_current_idx = trb.get_next_idx();
    trb.needs_commit = false;
  }
}

} // Util

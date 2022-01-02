#pragma once

// Since C++ has no concept of a runtime constant, implement our own.
template<typename T>
class LatchingValue {
public:
  // You can create a latching value with a value that... surprise... latches
  LatchingValue (T t)
                : m_value(t)
                , is_latched(true)
                {}
  // Or create one without an initial value, and latch it later
  LatchingValue ()
                : m_value()
                , is_latched(false)
                {}

  // Latch the value
  void latch(T t) {
    // no
    if (is_latched) {
      return;
    }
    m_value = t;
    is_latched = true;
  }

  // get the value
  const T& get() const {
    return m_value;
  }
private:
  T m_value;
  bool is_latched;
};
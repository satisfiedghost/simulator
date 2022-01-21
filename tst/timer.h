// A utility to time, maintain samples of, and run analysis on different points of execution

#include <algorithm>
#include <chrono>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

namespace chrono = std::chrono;

template <typename TIME>
class Timer {
public:
  Timer()
    : clock(chrono::steady_clock())
    {}

  void start() {
    start_time = clock.now();
  }

  void stop() {
    auto end_time = clock.now();
    auto elapsed = chrono::duration_cast<TIME>(end_time - start_time);
    samples.push_back(elapsed);
  }

  TIME calculate_average() {
    auto sum = std::accumulate(samples.begin(), samples.end(), TIME{0});
    return static_cast<TIME>(sum) / samples.size();
  }

  TIME calculate_median() {
    auto copy = samples;
    size_t n = copy.size() / 2;
    std::nth_element(copy.begin(), copy.begin() + n, copy.end());
    return copy[n];
  }

  TIME max() {
    return *std::max_element(samples.begin(), samples.end());
  }

  TIME min() {
    return *std::min_element(samples.begin(), samples.end());
  }

  void print_average() {
    std::cout << "Average: " << calculate_average().count() << unit_name() << std::endl;
  }

  void print_median() {
    std::cout << "Median: " << calculate_median().count() << unit_name() << std::endl;
  }

  void print_max() {
    std::cout << "Max: " << max().count() << unit_name() << std::endl;
  }

  void print_min() {
    std::cout << "Min: " << min().count() << unit_name() << std::endl;
  }

  void print_all() {
    print_average();
    print_median();
    print_max();
    print_min();
  }

private:
  std::string unit_name() {
    return "?";
  }

  std::vector<TIME> samples;
  const chrono::steady_clock& clock;
  chrono::steady_clock::time_point start_time;
};

template <>
inline std::string Timer<chrono::seconds>::unit_name() {
  return "s";
}

template <>
inline std::string Timer<chrono::milliseconds>::unit_name() {
  return "ms";
}

template <>
inline std::string Timer<chrono::microseconds>::unit_name() {
  return "us";
}

template <>
inline std::string Timer<chrono::nanoseconds>::unit_name() {
  return "ns";
}
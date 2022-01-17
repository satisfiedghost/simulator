#pragma once

#include <cmath>
#include <cinttypes>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <tuple>

namespace Util {

typedef __int128_t int128_t;

static_assert(sizeof(int128_t) == 16, "Your architecture may not support 128bit ints, 64 bit support may be added in the future");
// taken from https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
// under CC0 license
template<typename ... Args>
std::string string_format( const std::string& format, Args ... args ) {
    int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    auto size = static_cast<size_t>( size_s );
    auto buf = std::make_unique<char[]>( size );
    std::snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}

int128_t lllabs(int128_t);

class FixedPoint {
public:
  // TODO the scaling factor should be a power of 2. This allows us to scale with shifting operations instead of
  // multiplication. More efficient, but let's make something human-readable for now
  static constexpr int64_t DEFAULT_SCALING_FACTOR = 10'000'000ULL;

  static const __uint128_t UINT128_MAX =__uint128_t(__int128_t(-1L));
  static const __int128_t MAX = UINT128_MAX >> 1;
  static const __int128_t MIN = -MAX - 1;

  // Since numbers are by default multiplied by our scaling factor... we can't create one with
  // a magnitude greater than MAX or MAX divided by the scaling factor
  static constexpr int128_t MAX_PRESCALE = MAX / DEFAULT_SCALING_FACTOR;
  static constexpr int128_t MIN_PRESCALE = MIN / DEFAULT_SCALING_FACTOR;

  FixedPoint()
    : value(0)
    , scalar(DEFAULT_SCALING_FACTOR)
    {}

  FixedPoint(int32_t v)
    : value(pre_scale(v))
    , scalar(DEFAULT_SCALING_FACTOR)
    {}

  FixedPoint(size_t v)
    : FixedPoint(static_cast<int64_t>(v))
    {}

  // construct with a 64 bit number, checking for under/overflow
  FixedPoint(int64_t v)
    : value(pre_scale(v))
    , scalar(DEFAULT_SCALING_FACTOR)
    {}

  // construct an exactly accurate FixedPoint
  // this allows you to construct a number which was not fully representable in a float
  FixedPoint(int64_t whole, int64_t mantissa)
    : value(pre_scale(whole, mantissa))
    , scalar(DEFAULT_SCALING_FACTOR)
    {}

  FixedPoint(double d)
    : value(pre_scale(d))
    , scalar(DEFAULT_SCALING_FACTOR)
    {}

  double as_double() const {
    int128_t real_whole = value / DEFAULT_SCALING_FACTOR;
    int128_t real_mantissa = value % DEFAULT_SCALING_FACTOR;

    return static_cast<double>(real_whole) +
      static_cast<double>(real_mantissa) / static_cast<double>(DEFAULT_SCALING_FACTOR);
  }

  friend std::ostream& operator<<(std::ostream&, const FixedPoint&);

  friend std::istream& operator>>(std::istream&, FixedPoint&);

  // Analogues for the <cmath> functions we need access to
  friend FixedPoint pow(const FixedPoint&, const FixedPoint&);

  friend FixedPoint sqrt(const FixedPoint&);

  friend FixedPoint abs(const FixedPoint&);

  friend FixedPoint cos(const FixedPoint&);

  friend FixedPoint sin(const FixedPoint&);

  // Add two FixedPoints
  FixedPoint operator+(const FixedPoint&) const;
  // Subtract two FixedPoints
  FixedPoint operator-(const FixedPoint&) const;
  // Multiply two FixedPoints
  FixedPoint operator*(const FixedPoint&) const;
  // Divide two integers
  FixedPoint operator/(const FixedPoint&) const;
  // Compare two FixedPoints
  bool operator<(const FixedPoint&) const;
  // Compare two FixedPoints
  bool operator<=(const FixedPoint&) const;
  // Compare two FixedPoints
  bool operator>=(const FixedPoint&) const;
  // Compare two FixedPoints
  bool operator>(const FixedPoint&) const;
  // Equal
  bool operator==(const FixedPoint&) const;
  // Not equal
  bool operator!=(const FixedPoint&) const;
  // Add to this
  FixedPoint& operator+=(const FixedPoint&);
  // Unary -
  FixedPoint operator-() const;

  // convert to float
  explicit operator float() const { return static_cast<float>(as_double()); }

  // convert to double
  explicit operator double() const { return as_double(); }

private:
  // value e.g. 3.1415 is stored as 31,415
  int128_t value;
  // value by which this was scaled e.g. 31,415 has a scale of 10,000
  int32_t scalar;
  // pre-check input values
  int128_t pre_scale(int64_t value) const {
    return value * DEFAULT_SCALING_FACTOR;
  }

  int128_t pre_scale(int32_t value) const {
    // we know this fits comfortably
    return value * DEFAULT_SCALING_FACTOR;
  }

  int128_t pre_scale(int64_t whole, int64_t mantissa) const {
    // this is a bit painful... but switching to scaling with 2^n should allow a clever bitmath
    // solution instead....
    uint32_t divisor = 10;
    while (mantissa / divisor != 0) {
        divisor *= 10;
    }

    return whole * DEFAULT_SCALING_FACTOR + mantissa * (DEFAULT_SCALING_FACTOR / divisor);
  }

  int128_t pre_scale(double value) const {
    int128_t result;
    double frac = std::modf(value, &value);

    result = static_cast<int128_t>(value * DEFAULT_SCALING_FACTOR);
    frac *= DEFAULT_SCALING_FACTOR;

    result += static_cast<int128_t>(frac);
    return result;
  }

  int128_t pre_check_add(int128_t first, int128_t second) const {
    return first + second;
  }

  int128_t pre_check_sub(int128_t first, int128_t second) const {
    return first - second;
  }

  int128_t pre_check_mul(int128_t first, int128_t second) const {
    return first * second;
  }

  // Construct an FixedPoint with no scaling or pre-check
  // useful for operator overloads which have already calculated and checked the result
  enum unchecked_t {unchecked};
  FixedPoint(int128_t v, unchecked_t)
    : value(v)
    , scalar(DEFAULT_SCALING_FACTOR)
    {}
};

static const FixedPoint EPSILON = FixedPoint(1) / FixedPoint::DEFAULT_SCALING_FACTOR;

std::ostream& operator<<(std::ostream& os, const __int128_t i);
std::ostream& operator<<(std::ostream& os, const FixedPoint& i);


constexpr size_t calc_precision() {
  size_t s = 0;
  int64_t sf = FixedPoint::DEFAULT_SCALING_FACTOR;

  while(sf > 1) {
    sf /= 10;
    s++;
  }

  return s;
}

// Annoyingly, this line used to be the below (and inside the class above)
// static constexpr size_t PRECISION = static_cast<size_t>(std::log(DEFAULT_SCALING_FACTOR) / std::log(10));
// gcc, despite not being to the letter of the standard, allows evaluation of std::log as constexpr. clang more precisely
// conforms to the standard and refuses to do this. So instead, we have to manually calculate this.
constexpr size_t PRECISION = calc_precision();


} // Util
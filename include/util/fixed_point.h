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

class FixedPoint {
public:
  // if we're squaring numbers... then going larger than sqrt(int64_t::max) will cause overflow
  // TODO technically this should be additionally predicated on our # of dimensions, but not worrying about that for now
  // TODO the scaling factor should be a power of 2. This allows us to scale with shifting operations instead of
  // multiplication. Orders of magnitude more efficient, but let's make something human-readable for now
  static constexpr int32_t DEFAULT_SCALING_FACTOR = 10'000UL;
  static constexpr size_t PRECISION = std::log(DEFAULT_SCALING_FACTOR) / std::log(10);

  static constexpr int64_t MAX = std::numeric_limits<int32_t>::max();
  static constexpr int64_t MIN = std::numeric_limits<int32_t>::min();

  // Since squaring numbers greater than this would result in a signed 64-bit overflow, this is the
  // maximum value a non-intermediate number is allowed to have.
  static constexpr int32_t MAX_PRESCALE = std::numeric_limits<int32_t>::max() / DEFAULT_SCALING_FACTOR;
  static constexpr int32_t MIN_PRESCALE = std::numeric_limits<int32_t>::min() / DEFAULT_SCALING_FACTOR;

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
    int64_t real_whole = value / DEFAULT_SCALING_FACTOR;
    int64_t real_mantissa = value % DEFAULT_SCALING_FACTOR;

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
  int64_t value;
  // value by which this was scaled e.g. 31,415 has a scale of 10,000
  int32_t scalar;
  // pre-check input values
  int64_t pre_scale(int64_t value) const {
    if (value > 0 and (value * DEFAULT_SCALING_FACTOR > MAX or
         ((value * DEFAULT_SCALING_FACTOR < 0)))) {
        throw std::overflow_error(string_format("Int64 Prescale: Overflow creating FixedPoint with %d", value));
    } else if (value < 0 and (value * DEFAULT_SCALING_FACTOR < MIN or
         ((value * DEFAULT_SCALING_FACTOR > 0)))) {
        throw std::underflow_error(string_format("Int64 Prescale: Underflow creating FixedPoint with %d", value));
    }

    return value * DEFAULT_SCALING_FACTOR;
  }

  int64_t pre_scale(int32_t value) const {
    if (value > 0 and value > MAX_PRESCALE) {
      throw std::overflow_error(string_format("Int32 Prescale: Overflow creating FixedPoint with %d", value));
    } else if (value < 0 and value < MIN_PRESCALE) {
      throw std::underflow_error(string_format("Int32 Prescale: Underflow creating FixedPoint with %d", value));
    }
    return value * DEFAULT_SCALING_FACTOR;
  }

  int64_t pre_scale(int64_t whole, int64_t mantissa) const {
    if (whole > MAX_PRESCALE) {
      throw std::overflow_error(string_format("Int64 Whole/Mantissa Prescale: Overflow creating FixedPoint with %ld", value));
    } else if (value < MIN_PRESCALE) {
      throw std::underflow_error(string_format("Int64 Whole/Mantissa Prescale: Underflow creating FixedPoint with %ld", value));
    }

    // this is a bit painful... but switching to scaling with 2^n should allow a clever bitmath
    // solution instead....
    uint32_t divisor = 10;
    while (mantissa / divisor != 0) {
        divisor *= 10;
    }

    if (divisor > DEFAULT_SCALING_FACTOR) {
        throw std::overflow_error(string_format("Overflow on mantissa creating FixedPoint with %d . %d", whole, mantissa));
    }

    return whole * DEFAULT_SCALING_FACTOR + mantissa * (DEFAULT_SCALING_FACTOR / divisor);
  }

  int64_t pre_scale(double value) const {
    if (value > MAX_PRESCALE) {
      throw std::overflow_error(string_format("Double Prescale: Overflow creating FixedPoint with %f", value));
    } else if (value < MIN_PRESCALE) {
      throw std::underflow_error(string_format("Double Prescale: Underflow creating FixedPoint with %f", value));
    }

    int64_t result;
    double frac = std::modf(value, &value);
    result = static_cast<int64_t>(value * DEFAULT_SCALING_FACTOR);
    //std::cout << "Double prescale:: result: " << result << std::endl;
    //std::cout << "Double prescale:: frac(pre): " << frac << std::endl;;
    frac *= DEFAULT_SCALING_FACTOR;
    //std::cout << "Double prescale:: frac(post): " << frac << std::endl;;
    result += static_cast<int64_t>(frac);
    //std::cout << "Double prescale:: total: " << result << std::endl;
    return result;
  }

  int64_t pre_check(int64_t value) const {
    if (value > MAX) {
      throw std::overflow_error(string_format("Int64 PreCheck: Overflow creating FixedPoint with %ld", value));
    } else if (value < MIN) {
      throw std::underflow_error(string_format("Int64 PreCheck: Underflow creating FixedPoint with %ld", value));
    }
    return value;
  }

  // Construct an FixedPoint with no scaling or pre-check
  // useful for operator overloads which have already calculated and checked the result
  enum unchecked_t {unchecked};
  FixedPoint(int64_t v, unchecked_t)
    : value(v)
    , scalar(DEFAULT_SCALING_FACTOR)
    {}
};

} // Util
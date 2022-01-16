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
  // TODO the scaling factor should be a power of 2. This allows us to scale with shifting operations instead of
  // multiplication. More efficient, but let's make something human-readable for now
  static constexpr int64_t DEFAULT_SCALING_FACTOR = 10'000UL;
  static constexpr size_t PRECISION = std::log(DEFAULT_SCALING_FACTOR) / std::log(10);

  // We use these to check if operations would cause over/under flow
  static constexpr int64_t MAX = std::numeric_limits<int64_t>::max();
  static constexpr int64_t MIN = std::numeric_limits<int64_t>::min();

  // Since numbers are by default multiplied by our scaling factor... we can't create one with
  // a magnitude greater than MAX or MAX divided by the scaling factor
  static constexpr int64_t MAX_PRESCALE = MAX / DEFAULT_SCALING_FACTOR;
  static constexpr int64_t MIN_PRESCALE = MIN / DEFAULT_SCALING_FACTOR;

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
    if (value > MAX_PRESCALE) {
        throw std::overflow_error(string_format("Int64 Prescale: Overflow creating FixedPoint with %d", value));
    } else if (value < MIN_PRESCALE) {
        throw std::underflow_error(string_format("Int64 Prescale: Underflow creating FixedPoint with %d", value));
    }

    return value * DEFAULT_SCALING_FACTOR;
  }

  int64_t pre_scale(int32_t value) const {
    // we know this fits comfortably
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

  int64_t pre_check_add(int64_t first, int64_t second) const {
    if ((second > 0) and (first > MAX - second)) {
      throw std::overflow_error(string_format("Int64 PreCheckAdd: Overflow: %ld + %ld", first, second));
    } else if ((second < 0) and (first < MAX - second)) {
      throw std::underflow_error(string_format("Int64 PreCheckAdd: Underflow: %ld + %ld", first, second));
    }
    return first + second;
  }

  int64_t pre_check_sub(int64_t first, int64_t second) const {
    if ((second < 0) and (first > MAX + second)) {
      throw std::overflow_error(string_format("Int64 PreCheckSub: Overflow: %ld - %ld", first, second));
    } else if ((second > 0) and (first < MIN + second)) {
      throw std::underflow_error(string_format("Int64 PreCheckSub: Underflow: %ld - %ld", first, second));
    }
    return first - second;
  }

  int64_t pre_check_mul(int64_t first, int64_t second) const {
    if (first == 0 or second == 0) {
      return 0;
    }
    // general case
    if (first > MAX / std::labs(second)) {
      throw std::overflow_error(string_format("Int64 PreCheckMul: Overflow: %ld * %ld", first, second));
    } else if (first < MIN / std::labs(second)) {
      throw std::underflow_error(string_format("Int64 PreCheckMul: Underflow: %ld * %ld", first, second));
    }

    return first * second;
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
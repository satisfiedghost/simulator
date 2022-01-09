#include <cmath>
#include <cinttypes>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <tuple>

namespace Util {

// taken from https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
// under CC0 license
template<typename ... Args>
std::string string_format( const std::string& format, Args ... args )
{
    int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    auto size = static_cast<size_t>( size_s );
    auto buf = std::make_unique<char[]>( size );
    std::snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}

class Integer {
public:
  // if we're squaring numbers... then going larger than sqrt(int64_t::max) will cause overflow
  // TODO technically this should be additionally predicated on our # of dimensions, but not worrying about that for now
  // TODO the scaling factor should be a power of 2. This allows us to scale with shifting operations instead of
  // multiplication. Orders of magnitude more efficient, but let's make something human-readable for now
  static constexpr int32_t DEFAULT_SCALING_FACTOR = 10'000UL;
  static constexpr int64_t MAX = std::numeric_limits<int32_t>::max();
  static constexpr int32_t MAX_PRESCALE = std::numeric_limits<int32_t>::max() / DEFAULT_SCALING_FACTOR;
  static constexpr int64_t MIN = std::numeric_limits<int32_t>::min();
  static constexpr int32_t MIN_PRESCALE = std::numeric_limits<int32_t>::min() / DEFAULT_SCALING_FACTOR;

  Integer()
    : value(0)
    , scalar(DEFAULT_SCALING_FACTOR)
    {}

  Integer(int32_t v)
    : value(pre_scale(v))
    , scalar(DEFAULT_SCALING_FACTOR)
    {}


  // construct with a 64 bit number, checking for under/overflow
  Integer(int64_t v)
    : value(pre_scale(v))
    , scalar(DEFAULT_SCALING_FACTOR)
    {}

  // construct an exactly accurate Integer
  // this allows you to construct a number which was not fully representable in a float
  Integer(int64_t whole, int64_t mantissa)
    : value(pre_scale(whole, mantissa))
    , scalar(DEFAULT_SCALING_FACTOR)
    {}

  Integer(double d)
    : value(pre_scale(d))
    , scalar(DEFAULT_SCALING_FACTOR)
    {}

  // move me
  Integer (Integer&& other) {
    value = other.value;
    scalar = other.scalar;
  }

  // move assign me
  Integer& operator=(Integer&& other) {
    value = other.value;
    scalar = other.scalar;
    return *this;
  }

  double as_double() {
    int64_t real_whole = value / DEFAULT_SCALING_FACTOR;
    int64_t real_mantissa = value % DEFAULT_SCALING_FACTOR;

    return static_cast<double>(real_whole) +
      static_cast<double>(real_mantissa) / static_cast<double>(DEFAULT_SCALING_FACTOR);
  }

  friend std::ostream& operator<<(std::ostream&, const Integer&);

  friend Integer square(const Integer&);

  friend Integer sqrt(const Integer&);

  // Add two Integers
  Integer operator+(const Integer&) const;
  // Subtract two Integers
  Integer operator-(const Integer&) const;
  // Multiply two Integers
  Integer operator*(const Integer&) const;
  // Divide two integers
  Integer operator/(const Integer&) const;

private:
  // value e.g. 3.1415 is stored as 31,415
  int64_t value;
  // value by which this was scaled e.g. 31,415 has a scale of 10,000
  int32_t scalar;
  // pre-check input values
  int64_t pre_scale(int64_t value) const {
    if (value > 0 and (value * DEFAULT_SCALING_FACTOR > MAX or
         ((value * DEFAULT_SCALING_FACTOR < 0)))) {
        throw std::overflow_error(string_format("Int64 Prescale: Overflow creating Integer with %d", value));
    } else if (value < 0 and (value * DEFAULT_SCALING_FACTOR < MIN or
         ((value * DEFAULT_SCALING_FACTOR > 0)))) {
        throw std::underflow_error(string_format("Int64 Prescale: Underflow creating Integer with %d", value));
    }

    return value * DEFAULT_SCALING_FACTOR;
  }

  int64_t pre_scale(int32_t value) const {
    if (value > 0 and value > MAX_PRESCALE) {
      throw std::overflow_error(string_format("Int32 Prescale: Overflow creating Integer with %d", value));
    } else if (value < 0 and value < MIN_PRESCALE) {
      throw std::underflow_error(string_format("Int32 Prescale: Underflow creating Integer with %d", value));
    }
    return value * DEFAULT_SCALING_FACTOR;
  }

  int64_t pre_scale(int64_t whole, int64_t mantissa) const {
    if (whole > MAX_PRESCALE) {
      throw std::overflow_error(string_format("Int64 Whole/Mantissa Prescale: Overflow creating Integer with %d", value));
    } else if (value < MIN_PRESCALE) {
      throw std::underflow_error(string_format("Int64 Whole/Mantissa Prescale: Underflow creating Integer with %d", value));
    }

    // this is a bit painful... but switching to scaling with 2^n should allow a clever bitmath
    // solution instead....
    uint32_t divisor = 10;
    while (mantissa / divisor != 0) {
        divisor *= 10;
    }

    if (divisor > DEFAULT_SCALING_FACTOR) {
        throw std::overflow_error(string_format("Overflow on mantissa creating Integer with %d . %d", whole, mantissa));
    }

    return whole * DEFAULT_SCALING_FACTOR + mantissa * (DEFAULT_SCALING_FACTOR / divisor);
  }

  int64_t pre_scale(double value) const {
    if (value > MAX_PRESCALE) {
      throw std::overflow_error(string_format("Double Prescale: Overflow creating Integer with %f", value));
    } else if (value < MIN_PRESCALE) {
      throw std::underflow_error(string_format("Double Prescale: Underflow creating Integer with %f", value));
    }

    int64_t result;
    double frac = std::modf(value, &value);
    result = static_cast<int64_t>(value * DEFAULT_SCALING_FACTOR);
    frac *= DEFAULT_SCALING_FACTOR;
    result += static_cast<int64_t>(frac);
    return result;
  }

  int64_t pre_check(int64_t value) const {
    if (value > MAX) {
      throw std::overflow_error(string_format("Int64 PreCheck: Overflow creating Integer with %d", value));
    } else if (value < MIN) {
      throw std::underflow_error(string_format("Int64 PreCheck: Underflow creating Integer with %d", value));
    }
    return value;
  }

  // Construct an Integer with no scaling or pre-check
  // useful for operator overloads which have already calculated and checked the result
  enum unchecked_t {unchecked};
  Integer(int64_t v, unchecked_t)
    : value(v)
    , scalar(DEFAULT_SCALING_FACTOR)
    {}
};

Integer Integer::operator+(const Integer& other) const {
  int64_t result = pre_check(value + other.value);
  return Integer(result, unchecked);
}

Integer Integer::operator-(const Integer& other) const {
  int64_t result = pre_check(value - other.value);
  return Integer(result, unchecked);
}

Integer Integer::operator*(const Integer& other) const {
    // the intermediate result of this operation might be much larger than our allowed representation.
    // that is fine! it is still guaranteed to fit in an int64_t due to our enforcement of a MAX/MIN
    // we will however lose some precision moving back to the fixed scale
    int64_t result = value * other.value;
    result /= DEFAULT_SCALING_FACTOR;

    return Integer(result, unchecked);
}

Integer Integer::operator/(const Integer& other) const {
    // Dividing these numbers as-is would result in a great loss of precision. e.g. if we have
    // 1.5 / 2, we want 0.75. Internally, 1500 / 2000 = 0. We get around this by scaling the dividend up
    // (using 10k as an example factor) as an intermediate step. Then (1500 * 10'000) / 2000 = 7500. With
    // a 10k scaling interpretation, this is 0.75
    int64_t dividend = value * DEFAULT_SCALING_FACTOR;
    int64_t result = dividend / other.value;
    return Integer(result, unchecked);
}

Integer square(const Integer& i) {
  return i * i;
}

Integer sqrt(const Integer& i) {

}


std::ostream& operator<<(std::ostream& os, const Integer& i) {
  auto whole = i.value / i.scalar;
  auto mantissa = i.value % i.scalar;

  os << ( (whole < 0) ? "-" : "" ) << std::abs(whole) << "." << std::abs(mantissa);
  return os;
}

} // Util
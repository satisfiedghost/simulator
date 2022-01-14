#include "util/fixed_point.h"

namespace Util {

FixedPoint FixedPoint::operator+(const FixedPoint& other) const {
  int64_t result = pre_check(value + other.value);
  return FixedPoint(result, unchecked);
}

FixedPoint FixedPoint::operator-(const FixedPoint& other) const {
  int64_t result = pre_check(value - other.value);
  return FixedPoint(result, unchecked);
}

bool FixedPoint::operator<(const FixedPoint& other) const {
  return value < other.value;
}

bool FixedPoint::operator<=(const FixedPoint& other) const {
  return value <= other.value;
}

bool FixedPoint::operator>(const FixedPoint& other) const {
  return value > other.value;
}

bool FixedPoint::operator>=(const FixedPoint& other) const {
  return value >= other.value;
}

FixedPoint FixedPoint::operator*(const FixedPoint& other) const {
    // the intermediate result of this operation might be much larger than our allowed representation.
    // that is fine! it is still guaranteed to fit in an int64_t due to our enforcement of a MAX/MIN
    // we will however lose some precision moving back to the fixed scale
    int64_t result = value * other.value;
    result /= DEFAULT_SCALING_FACTOR;

    return FixedPoint(result, unchecked);
}

FixedPoint FixedPoint::operator/(const FixedPoint& other) const {
    // Dividing these numbers as-is would result in a great loss of precision. e.g. if we have
    // 1.5 / 2, we want 0.75. Internally, 1500 / 2000 = 0. We get around this by scaling the dividend up
    // (using 10k as an example factor) as an intermediate step. Then (1500 * 10'000) / 2000 = 7500. With
    // a 10k scaling interpretation, this is 0.75
    int64_t dividend = value * DEFAULT_SCALING_FACTOR;
    int64_t result = dividend / other.value;
    return FixedPoint(result, unchecked);
}

FixedPoint FixedPoint::operator-() const {
  return FixedPoint(-1) * (*this);
}

FixedPoint pow(const FixedPoint& i, const FixedPoint& pow) {
  if (pow == FixedPoint(0) or i == FixedPoint(0)) {
    return FixedPoint(1);
  }

  FixedPoint result(i);

  // TODO implement assignment & operators with integer literals
  for (FixedPoint j(1); j < abs(pow); j = j + FixedPoint(1)) {
    result = result * i;
  }

  return (pow > FixedPoint(0)) ? result : FixedPoint(1) / result;
}

FixedPoint cos(const FixedPoint& f) {
  return FixedPoint(std::cos(f.as_double()));
}

FixedPoint sin(const FixedPoint& f) {
  return FixedPoint(std::sin(f.as_double()));
}

bool FixedPoint::operator==(const FixedPoint& other) const {
  return value == other.value;
}

bool FixedPoint::operator!=(const FixedPoint& other) const {
  return value != other.value;
}

FixedPoint abs(const FixedPoint& i) {
  return FixedPoint(std::abs(i.value), FixedPoint::unchecked);
}

FixedPoint sqrt(const FixedPoint& i) {
  if (i.value == 0) {
    return FixedPoint(0);
  }

  // Take a guess
  FixedPoint guess = i / 2;

  // Newton's Method step for sqrt(x)
  auto NR = [&]() -> FixedPoint {
    return guess - (guess * guess - i) / (FixedPoint(2) * guess);
  };

  FixedPoint next;
  FixedPoint delta;
  const FixedPoint min(0.0001);

  do {
    next = NR();
    delta = abs(guess - next);
    guess = std::move(next);
  } while (min < delta);

  return guess;
}


std::ostream& operator<<(std::ostream& os, const FixedPoint& i) {
  auto whole = i.value / i.scalar;
  auto mantissa = i.value % i.scalar;

  os << ( (whole < 0) ? "-" : "" ) << std::abs(whole) << "." << std::setw(4) << std::setfill('0') << std::abs(mantissa);
  return os;
}

} // Util
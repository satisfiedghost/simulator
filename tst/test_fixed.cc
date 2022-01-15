#include "util/fixed_point.h"
#include <gtest/gtest.h>

using Util::FixedPoint;
// Allows us to use ADL to defer to the correct sqrt/pow impls
using std::sqrt;
using std::pow;
using std::abs;

class FixedTest : public ::testing::Test {
public:
static constexpr double PRECISION = 1.f / static_cast<double>(FixedPoint::DEFAULT_SCALING_FACTOR);
void addition_test(int64_t a, int64_t b) {
  FixedPoint one(a);
  FixedPoint two(b);
  int64_t expected = a + b;

  FixedPoint c = one + two;

  ASSERT_FLOAT_EQ(c.as_double(), expected);
}

void subtraction_test(int64_t a, int64_t b) {
  FixedPoint one(a);
  FixedPoint two(b);
  int64_t expected = a - b;

  FixedPoint c = one - two;

  ASSERT_FLOAT_EQ(c.as_double(), expected);
}

void multiplication_test(int64_t a, int64_t b) {
  FixedPoint one(a);
  FixedPoint two(b);
  int64_t expected = a * b;

  FixedPoint c = one * two;

  ASSERT_FLOAT_EQ(c.as_double(), expected);
}

void division_test(int64_t a, int64_t b) {
  FixedPoint one(a);
  FixedPoint two(b);
  double expected = static_cast<double>(a) / static_cast<double>(b);

  FixedPoint c = one / two;

  EXPECT_NEAR(c.as_double(), expected, PRECISION);
}

void pow_test(int64_t a, int32_t power) {
  FixedPoint i(a);
  FixedPoint p(power);
  double expected = std::pow(a, power);

  FixedPoint r = pow(a, p);

  EXPECT_NEAR(r.as_double(), expected, PRECISION);
}

void square_root_test(double a) {
  FixedPoint i(a);
  double expected = std::sqrt(a);

  FixedPoint p = sqrt(a);

  EXPECT_NEAR(p.as_double(), expected, PRECISION);
}

};

TEST_F(FixedTest, BasicAddition) {
  for (int i = 1; i < 10; i++) {
    addition_test(5 * i, 10 * i);
    addition_test(0 * i, 0 * i);
    addition_test(5 * i, 0 * i);
    addition_test(0 * i, 5 * i);
    addition_test(-5 * i, 10 * i);
    addition_test(5 * i, -10 * i);
    addition_test(-5 * i, -5 * i);
  }
}

TEST_F(FixedTest, BasicSubtraction) {
  for (int i = 1; i < 10; i++) {
    subtraction_test(5 * i, 10 * i);
    subtraction_test(0 * i, 0 * i);
    subtraction_test(5 * i, 0 * i);
    subtraction_test(0 * i, 5 * i);
    subtraction_test(-5 * i, 10 * i);
    subtraction_test(5 * i, -10 * i);
    subtraction_test(-5 * i, -5 * i);
  }
}

TEST_F(FixedTest, BasicMultiplication) {
  for (int i = 1; i < 10; i++) {
    multiplication_test(5 * i, 10 * i);
    multiplication_test(0 * i, 0 * i);
    multiplication_test(5 * i, 0 * i);
    multiplication_test(0 * i, 5 * i);
    multiplication_test(-5 * i, 10 * i);
    multiplication_test(5 * i, -10 * i);
    multiplication_test(-5 * i, -5 * i);
  }
}

TEST_F(FixedTest, BasicDivision) {
  for (int i = 1; i < 10; i++) {
    division_test(5 * i, 10 * i);
    division_test(-5 * i, 10 * i);
    division_test(5 * i, -10 * i);
    division_test(-5 * i, -5 * i);
    division_test(10 * i, 3 * i);
  }
}

TEST_F(FixedTest, BasicPow) {
  for (int i = 0; i < 5; i++) {
    pow_test(5 * i, i);
    //pow_test(0 * i, i);
    pow_test(5 * i, i);
    //pow_test(0 * i, i);
    pow_test(-5 * i, i);
    pow_test(5 * i, i);
    pow_test(-5 * i, i);
  }
}

TEST_F(FixedTest, BasicSquareRoot) {
  for (int i = 1; i < 10; i++) {
    square_root_test(0 * i);
    square_root_test(2.5 * i);
    square_root_test(5 * i);
    square_root_test(7.5 * i);
    square_root_test(10 * i);
  }
}

TEST_F(FixedTest, SquareLarge) {
  double large = 673;
  pow_test(large, 2);
}

TEST_F(FixedTest, MultiplyLarge) {
  for (double d = 0; d < 700; d++) {
    std::cout << d << std::endl;
    auto expected = d * d;
    FixedPoint a(d);
    a = a * a;
    EXPECT_NEAR(a.as_double(), expected, FixedTest::PRECISION);
  }
}
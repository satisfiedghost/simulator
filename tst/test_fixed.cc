#include "util/integer.h"
#include <gtest/gtest.h>

using Util::Integer;

class FixedTest : public ::testing::Test {
static constexpr double PRECISION = 1.f / static_cast<double>(Integer::DEFAULT_SCALING_FACTOR);
public:
void addition_test(int64_t a, int64_t b) {
  Integer one(a);
  Integer two(b);
  int64_t expected = a + b;

  Integer c = one + two;

  ASSERT_FLOAT_EQ(c.as_double(), expected);
}

void subtraction_test(int64_t a, int64_t b) {
  Integer one(a);
  Integer two(b);
  int64_t expected = a - b;

  Integer c = one - two;

  ASSERT_FLOAT_EQ(c.as_double(), expected);
}

void multiplication_test(int64_t a, int64_t b) {
  Integer one(a);
  Integer two(b);
  int64_t expected = a * b;

  Integer c = one * two;

  ASSERT_FLOAT_EQ(c.as_double(), expected);
}

void division_test(int64_t a, int64_t b) {
  Integer one(a);
  Integer two(b);
  double expected = static_cast<double>(a) / static_cast<double>(b);

  Integer c = one / two;

  EXPECT_NEAR(c.as_double(), expected, PRECISION);
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

TEST_F(FixedTest, Square) {
}

TEST_F(FixedTest, SquareRoot) {
}
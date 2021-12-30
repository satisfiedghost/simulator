#include <gtest/gtest.h>
#include "vector.h"
#include <vector>

std::vector<float> ones{1, -1};

class CombinationsTest :
    public ::testing::TestWithParam<std::tuple<float, float, float>> {};

// Magnitude Calculation
TEST_P(CombinationsTest, Magnitude) {
  Simulation::Vector<float> test1(std::get<0>(GetParam()),
                                  std::get<1>(GetParam()),
                                  std::get<2>(GetParam()));
  ASSERT_FLOAT_EQ(test1.magnitude, std::sqrt(3));
}

INSTANTIATE_TEST_SUITE_P(AllCombinations,
                        CombinationsTest,
                        ::testing::Combine(::testing::ValuesIn(ones),
                                           ::testing::ValuesIn(ones),
                                           ::testing::ValuesIn(ones)));


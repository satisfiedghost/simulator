#include "component.h"
#include "util/fixed_point.h"

#include <gtest/gtest.h>
#include <vector>

std::vector<float> ones{1, -1};

class CombinationsTest :
    public ::testing::TestWithParam<std::tuple<float, float, float>> {
public:
    static constexpr double PRECISION = 1.01f / static_cast<double>(Util::FixedPoint::DEFAULT_SCALING_FACTOR);
};


class FixedPointVectorTest : public ::testing::Test {
public:
static constexpr double PRECISION = 1.01f / static_cast<double>(Util::FixedPoint::DEFAULT_SCALING_FACTOR);
};


// TODO how to instantiate this with combinations?
//typedef ::testing::Types<float, double, Util::FixedPoint> MyTypes ;


// Magnitude Calculation
TEST_P(CombinationsTest, Magnitude) {
  Component::Vector<Util::FixedPoint> test1(std::get<0>(GetParam()),
                                      std::get<1>(GetParam()),
                                      std::get<2>(GetParam()));
  EXPECT_NEAR(test1.magnitude.as_double(), std::sqrt(3), PRECISION);
}

INSTANTIATE_TEST_SUITE_P(AllCombinations,
                        CombinationsTest,
                        ::testing::Combine(::testing::ValuesIn(ones),
                                           ::testing::ValuesIn(ones),
                                           ::testing::ValuesIn(ones)));

TEST_F(FixedPointVectorTest, UnitVector) {
  Component::Vector<Util::FixedPoint> v2D(5, 2, 0);
  Component::Vector<Util::FixedPoint> v3D(5, 2, -8);

  auto v2D_unit = v2D.unit_vector();
  auto v3D_unit = v3D.unit_vector();

  EXPECT_NEAR(v2D_unit.magnitude.as_double(), 1, PRECISION) <<
  "original vector: " << v2D << std::endl << "unit vector: " << v2D_unit << std::endl;
  EXPECT_NEAR(v3D_unit.magnitude.as_double(), 1, PRECISION) <<
  "original vector: " << v3D << std::endl << "unit vector: " << v3D_unit << std::endl;
}
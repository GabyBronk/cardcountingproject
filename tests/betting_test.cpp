#include <gtest/gtest.h>
#include "betting.h"

TEST(RecommendedUnits, NegativeEdgeBetsOneUnit) { EXPECT_EQ(1, recommendedUnits(-1.5)); }
TEST(RecommendedUnits, ZeroEdgeBetsOneUnit) { EXPECT_EQ(1, recommendedUnits(0.0)); }
TEST(RecommendedUnits, TinyPositiveEdgeStillOneUnit) { EXPECT_EQ(1, recommendedUnits(0.3)); }
TEST(RecommendedUnits, HalfPercentEdgeBetsTwoUnits) { EXPECT_EQ(2, recommendedUnits(0.5)); }
TEST(RecommendedUnits, TwoPercentEdgeBetsFiveUnits) { EXPECT_EQ(5, recommendedUnits(2.0)); }
TEST(RecommendedUnits, HugeEdgeIsCappedAtTwelveUnits) { EXPECT_EQ(12, recommendedUnits(50.0)); }

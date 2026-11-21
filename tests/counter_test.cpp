#include <gtest/gtest.h>
#include "counter.h"

namespace {
Card makeCard(int rank) { return Card{rank, Suit::Hearts}; }

Counter counterAfterSeeing(int rank, int times) {
    Counter counter;
    for (int i = 0; i < times; ++i) counter.see(makeCard(rank));
    return counter;
}
}  // namespace

TEST(Counter, StartsAtZero) { EXPECT_EQ(0, Counter().runningCount()); }
TEST(Counter, LowCardRaisesCount) { EXPECT_EQ(1, counterAfterSeeing(4, 1).runningCount()); }
TEST(Counter, HighCardLowersCount) { EXPECT_EQ(-1, counterAfterSeeing(13, 1).runningCount()); }
TEST(Counter, NeutralCardLeavesCountUnchanged) { EXPECT_EQ(0, counterAfterSeeing(8, 1).runningCount()); }
TEST(Counter, CountAccumulatesAcrossCards) { EXPECT_EQ(4, counterAfterSeeing(2, 4).runningCount()); }

TEST(Counter, ResetClearsCount) {
    Counter counter = counterAfterSeeing(2, 4);
    counter.reset();
    EXPECT_EQ(0, counter.runningCount());
}

TEST(Counter, FullDeckEndsAtZero) {
    Counter counter;
    for (const Card& card : buildDecks(1)) counter.see(card);
    EXPECT_EQ(0, counter.runningCount());
}

TEST(DecksRemaining, HalfADeck) { EXPECT_DOUBLE_EQ(0.5, decksRemaining(26)); }
TEST(DecksRemaining, FourDecks) { EXPECT_DOUBLE_EQ(4.0, decksRemaining(208)); }

TEST(TrueCount, DividesRunningCountByDecksRemaining) {
    EXPECT_DOUBLE_EQ(2.0, counterAfterSeeing(2, 4).trueCount(104));
}

TEST(TrueCount, SameRunningCountIsStrongerWithFewerDecks) {
    EXPECT_DOUBLE_EQ(4.0, counterAfterSeeing(2, 4).trueCount(52));
}

TEST(TrueCount, NegativeRunningCountGivesNegativeTrueCount) {
    EXPECT_DOUBLE_EQ(-2.0, counterAfterSeeing(10, 4).trueCount(104));
}

TEST(TrueCount, IsZeroWhenNoCardsRemain) {
    EXPECT_DOUBLE_EQ(0.0, counterAfterSeeing(2, 4).trueCount(0));
}

#include <gtest/gtest.h>
#include "hand.h"

namespace {
Hand handOf(const std::vector<int>& ranks) {
    Hand hand;
    for (int rank : ranks) hand.add(Card{rank, Suit::Spades});
    return hand;
}
}  // namespace

TEST(Hand, EmptyHandTotalsZero) { EXPECT_EQ(0, Hand().total()); }
TEST(Hand, NumberCardsAddUp) { EXPECT_EQ(11, handOf({5, 6}).total()); }
TEST(Hand, FaceCardsCountTen) { EXPECT_EQ(20, handOf({13, 12}).total()); }
TEST(Hand, AceCountsElevenWhenSafe) { EXPECT_EQ(17, handOf({1, 6}).total()); }
TEST(Hand, AceDropsToOneToAvoidBust) { EXPECT_EQ(17, handOf({1, 6, 10}).total()); }
TEST(Hand, TwoAcesMakeTwelve) { EXPECT_EQ(12, handOf({1, 1}).total()); }
TEST(Hand, TwoAcesAndNineMakeTwentyOne) { EXPECT_EQ(21, handOf({1, 1, 9}).total()); }

TEST(Hand, AceAndSixIsSoft) { EXPECT_TRUE(handOf({1, 6}).isSoft()); }
TEST(Hand, AceForcedToOneIsNotSoft) { EXPECT_FALSE(handOf({1, 6, 10}).isSoft()); }
TEST(Hand, NoAceIsNotSoft) { EXPECT_FALSE(handOf({10, 6}).isSoft()); }

TEST(Hand, TwentyTwoIsBust) { EXPECT_TRUE(handOf({10, 6, 6}).isBust()); }
TEST(Hand, TwentyOneIsNotBust) { EXPECT_FALSE(handOf({10, 5, 6}).isBust()); }

TEST(Hand, AceAndKingIsBlackjack) { EXPECT_TRUE(handOf({1, 13}).isBlackjack()); }
TEST(Hand, ThreeCardTwentyOneIsNotBlackjack) { EXPECT_FALSE(handOf({7, 7, 7}).isBlackjack()); }
TEST(Hand, TwentyWithTwoCardsIsNotBlackjack) { EXPECT_FALSE(handOf({10, 13}).isBlackjack()); }

TEST(Hand, ClearRemovesAllCards) {
    Hand hand = handOf({2, 3});
    hand.clear();
    EXPECT_TRUE(hand.cards().empty());
}

TEST(Hand, ToStringListsCardsInOrder) { EXPECT_EQ("AS KS", handOf({1, 13}).toString()); }
TEST(Hand, ToStringOfEmptyHandIsEmpty) { EXPECT_EQ("", Hand().toString()); }

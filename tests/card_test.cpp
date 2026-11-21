#include <gtest/gtest.h>
#include <algorithm>
#include "card.h"

namespace {
Card makeCard(int rank) { return Card{rank, Suit::Spades}; }

int countOf(const std::vector<Card>& cards, const Card& target) {
    return static_cast<int>(std::count(cards.begin(), cards.end(), target));
}
}  // namespace

TEST(BlackjackValue, NumberCardUsesFaceValue) { EXPECT_EQ(7, blackjackValue(makeCard(7))); }
TEST(BlackjackValue, TenIsTen) { EXPECT_EQ(10, blackjackValue(makeCard(10))); }
TEST(BlackjackValue, QueenIsTen) { EXPECT_EQ(10, blackjackValue(makeCard(12))); }
TEST(BlackjackValue, AceIsEleven) { EXPECT_EQ(11, blackjackValue(makeCard(1))); }

TEST(HiLo, TwoIsPlusOne) { EXPECT_EQ(1, hiLoValue(makeCard(2))); }
TEST(HiLo, SixIsPlusOne) { EXPECT_EQ(1, hiLoValue(makeCard(6))); }
TEST(HiLo, SevenIsZero) { EXPECT_EQ(0, hiLoValue(makeCard(7))); }
TEST(HiLo, NineIsZero) { EXPECT_EQ(0, hiLoValue(makeCard(9))); }
TEST(HiLo, TenIsMinusOne) { EXPECT_EQ(-1, hiLoValue(makeCard(10))); }
TEST(HiLo, KingIsMinusOne) { EXPECT_EQ(-1, hiLoValue(makeCard(13))); }
TEST(HiLo, AceIsMinusOne) { EXPECT_EQ(-1, hiLoValue(makeCard(1))); }

TEST(CardToString, AceOfSpades) { EXPECT_EQ("AS", cardToString(Card{1, Suit::Spades})); }
TEST(CardToString, TenOfHearts) { EXPECT_EQ("10H", cardToString(Card{10, Suit::Hearts})); }
TEST(CardToString, InvalidRankShowsQuestionMark) {
    EXPECT_EQ("?C", cardToString(Card{99, Suit::Clubs}));
}

TEST(BuildDecks, OneDeckHas52Cards) { EXPECT_EQ(52u, buildDecks(1).size()); }
TEST(BuildDecks, FourDecksHave208Cards) { EXPECT_EQ(208u, buildDecks(4).size()); }
TEST(BuildDecks, ZeroDecksIsEmpty) { EXPECT_TRUE(buildDecks(0).empty()); }

TEST(BuildDecks, OneDeckHasOneAceOfSpades) {
    EXPECT_EQ(1, countOf(buildDecks(1), Card{1, Suit::Spades}));
}

TEST(BuildDecks, TwoDecksHaveTwoAcesOfSpades) {
    EXPECT_EQ(2, countOf(buildDecks(2), Card{1, Suit::Spades}));
}

TEST(BuildDecks, HiLoValuesSumToZeroPerDeck) {
    int sum = 0;
    for (const Card& card : buildDecks(1)) sum += hiLoValue(card);
    EXPECT_EQ(0, sum);
}

TEST(IsValidDeckCount, OneIsValid) { EXPECT_TRUE(isValidDeckCount(1)); }
TEST(IsValidDeckCount, TwoIsValid) { EXPECT_TRUE(isValidDeckCount(2)); }
TEST(IsValidDeckCount, FourIsValid) { EXPECT_TRUE(isValidDeckCount(4)); }
TEST(IsValidDeckCount, ThreeIsInvalid) { EXPECT_FALSE(isValidDeckCount(3)); }
TEST(IsValidDeckCount, ZeroIsInvalid) { EXPECT_FALSE(isValidDeckCount(0)); }
TEST(IsValidDeckCount, EightIsInvalid) { EXPECT_FALSE(isValidDeckCount(8)); }

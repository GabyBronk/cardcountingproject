#include <gtest/gtest.h>
#include <stdexcept>
#include "shoe.h"

namespace {
Card makeCard(int rank) { return Card{rank, Suit::Clubs}; }

std::vector<Card> drawMany(Shoe& shoe, int count) {
    std::vector<Card> drawn;
    for (int i = 0; i < count; ++i) drawn.push_back(shoe.draw());
    return drawn;
}

void drawAndDiscard(Shoe& shoe, int count) {
    for (int i = 0; i < count; ++i) shoe.discard(shoe.draw());
}
}  // namespace

TEST(Shoe, TwoDeckShoeStartsWith104Cards) { EXPECT_EQ(104, Shoe(2, 1).cardsRemaining()); }
TEST(Shoe, ReportsDeckCount) { EXPECT_EQ(4, Shoe(4, 1).numDecks()); }
TEST(Shoe, NewShoeIsNotEmpty) { EXPECT_FALSE(Shoe(1, 1).isEmpty()); }

TEST(Shoe, DrawRemovesOneCard) {
    Shoe shoe(1, 1);
    shoe.draw();
    EXPECT_EQ(51, shoe.cardsRemaining());
}

TEST(Shoe, DrawFollowsQueueOrder) {
    Shoe shoe(std::vector<Card>{makeCard(5), makeCard(9)});
    EXPECT_EQ(5, shoe.draw().rank);
}

TEST(Shoe, SecondDrawIsSecondCard) {
    Shoe shoe(std::vector<Card>{makeCard(5), makeCard(9)});
    shoe.draw();
    EXPECT_EQ(9, shoe.draw().rank);
}

TEST(Shoe, DrawOnEmptyShoeThrows) {
    Shoe shoe(std::vector<Card>{});
    EXPECT_THROW(shoe.draw(), std::runtime_error);
}

TEST(Shoe, IsEmptyAfterDrawingEverything) {
    Shoe shoe(1, 1);
    drawMany(shoe, 52);
    EXPECT_TRUE(shoe.isEmpty());
}

TEST(Shoe, ShuffledShoeContainsEveryCardOnce) {
    Shoe shoe(1, 42);
    std::vector<Card> drawn = drawMany(shoe, 52);
    for (const Card& card : buildDecks(1))
        EXPECT_EQ(1, std::count(drawn.begin(), drawn.end(), card));
}

TEST(Shoe, SameSeedGivesSameOrder) {
    Shoe a(1, 7), b(1, 7);
    EXPECT_EQ(drawMany(a, 52), drawMany(b, 52));
}

TEST(Shoe, DifferentSeedsGiveDifferentOrder) {
    Shoe a(1, 7), b(1, 8);
    EXPECT_NE(drawMany(a, 52), drawMany(b, 52));
}

TEST(Shoe, ShuffleChangesOrderFromFreshDeck) {
    Shoe shoe(1, 7);
    EXPECT_NE(buildDecks(1), drawMany(shoe, 52));
}

TEST(Shoe, DiscardGrowsDiscardPile) {
    Shoe shoe(1, 1);
    shoe.discard(shoe.draw());
    EXPECT_EQ(1, shoe.discardCount());
}

TEST(Shoe, DiscardDoesNotReturnCardToShoe) {
    Shoe shoe(1, 1);
    shoe.discard(shoe.draw());
    EXPECT_EQ(51, shoe.cardsRemaining());
}

TEST(Shoe, ReshuffleBringsDiscardsBackIntoShoe) {
    Shoe shoe(1, 3);
    drawAndDiscard(shoe, 10);
    shoe.reshuffle();
    EXPECT_EQ(52, shoe.cardsRemaining());
}

TEST(Shoe, ReshuffleEmptiesDiscardPile) {
    Shoe shoe(1, 3);
    drawAndDiscard(shoe, 10);
    shoe.reshuffle();
    EXPECT_EQ(0, shoe.discardCount());
}

TEST(Shoe, ReshuffleWithNothingDiscardedKeepsShoeSize) {
    Shoe shoe(1, 3);
    shoe.reshuffle();
    EXPECT_EQ(52, shoe.cardsRemaining());
}

TEST(Shoe, ReshuffleRefillsAnEmptyShoeFromDiscards) {
    Shoe shoe(1, 3);
    drawAndDiscard(shoe, 52);
    shoe.reshuffle();
    EXPECT_FALSE(shoe.isEmpty());
}

TEST(Shoe, NoReshuffleNeededWhenFull) { EXPECT_FALSE(Shoe(1, 1).needsReshuffle()); }

TEST(Shoe, NoReshuffleNeededAtExactCutLine) {
    Shoe shoe(1, 1);
    drawMany(shoe, 39);  // 13 cards left = exactly 25%
    EXPECT_FALSE(shoe.needsReshuffle());
}

TEST(Shoe, ReshuffleNeededBelowCutLine) {
    Shoe shoe(1, 1);
    drawMany(shoe, 40);  // 12 cards left
    EXPECT_TRUE(shoe.needsReshuffle());
}

#include <gtest/gtest.h>
#include <sstream>
#include "ui.h"

namespace {
Game riggedGame(const std::vector<int>& ranks) {
    std::vector<Card> cards;
    for (int rank : ranks) cards.push_back(Card{rank, Suit::Clubs});
    return Game(Shoe(cards), 1000.0);
}

std::string runScripted(const std::string& input, Game& game, const EdgeTable& table) {
    std::istringstream in(input);
    std::ostringstream out;
    runApp(in, out, game, table);
    return out.str();
}

bool contains(const std::string& text, const std::string& part) {
    return text.find(part) != std::string::npos;
}
}  // namespace

TEST(ParseAction, HitFromH) { EXPECT_EQ(Action::Hit, parseAction("h")); }
TEST(ParseAction, StandFromS) { EXPECT_EQ(Action::Stand, parseAction("s")); }
TEST(ParseAction, DoubleFromD) { EXPECT_EQ(Action::Double, parseAction("d")); }
TEST(ParseAction, QuitFromQ) { EXPECT_EQ(Action::Quit, parseAction("q")); }
TEST(ParseAction, IgnoresCase) { EXPECT_EQ(Action::Hit, parseAction("HIT")); }
TEST(ParseAction, IgnoresLeadingSpaces) { EXPECT_EQ(Action::Stand, parseAction("   stand")); }
TEST(ParseAction, UnknownLetterIsUnknown) { EXPECT_EQ(Action::Unknown, parseAction("x")); }
TEST(ParseAction, EmptyInputIsUnknown) { EXPECT_EQ(Action::Unknown, parseAction("")); }

TEST(ParseNumber, ParsesWholeNumber) { EXPECT_DOUBLE_EQ(25.0, *parseNumber("25")); }
TEST(ParseNumber, ParsesDecimal) { EXPECT_DOUBLE_EQ(12.5, *parseNumber("12.5")); }
TEST(ParseNumber, IgnoresSurroundingSpaces) { EXPECT_DOUBLE_EQ(25.0, *parseNumber("  25  ")); }
TEST(ParseNumber, RejectsLetters) { EXPECT_FALSE(parseNumber("abc").has_value()); }
TEST(ParseNumber, RejectsTrailingGarbage) { EXPECT_FALSE(parseNumber("25abc").has_value()); }
TEST(ParseNumber, RejectsEmptyString) { EXPECT_FALSE(parseNumber("").has_value()); }

TEST(ParseWholeNumber, ParsesInteger) { EXPECT_EQ(4, *parseWholeNumber("4")); }
TEST(ParseWholeNumber, RejectsDecimal) { EXPECT_FALSE(parseWholeNumber("2.5").has_value()); }
TEST(ParseWholeNumber, RejectsHugeNumber) { EXPECT_FALSE(parseWholeNumber("99999999999999").has_value()); }

TEST(BettingInfo, ShowsNotAvailableWhenTableHasNoRow) {
    std::ostringstream out;
    printBettingInfo(out, riggedGame({}), EdgeTable());
    EXPECT_TRUE(contains(out.str(), "n/a"));
}

TEST(BettingInfo, ShowsBankroll) {
    std::ostringstream out;
    printBettingInfo(out, riggedGame({}), EdgeTable());
    EXPECT_TRUE(contains(out.str(), "$1000.00"));
}

TEST(RunApp, QuittingAtBetPromptEndsTheGame) {
    Game game = riggedGame({10, 10, 8, 9});
    EXPECT_TRUE(contains(runScripted("q\n", game, EdgeTable()), "Final bankroll: $1000.00"));
}

TEST(RunApp, InvalidBetIsRejectedWithMessage) {
    Game game = riggedGame({10, 10, 8, 9});
    EXPECT_TRUE(contains(runScripted("1\nq\n", game, EdgeTable()), "Invalid bet"));
}

TEST(RunApp, PlayingAHandReportsTheResult) {
    Game game = riggedGame({10, 10, 8, 9});
    EXPECT_TRUE(contains(runScripted("10\ns\nq\n", game, EdgeTable()), "Dealer wins."));
}

TEST(RunApp, LossUpdatesFinalBankroll) {
    Game game = riggedGame({10, 10, 8, 9});
    EXPECT_TRUE(contains(runScripted("10\ns\nq\n", game, EdgeTable()), "Final bankroll: $990.00"));
}

TEST(RunApp, UnavailableActionShowsMessage) {
    Game game = riggedGame({2, 9, 3, 7, 4, 10});
    EXPECT_TRUE(contains(runScripted("10\nx\ns\nq\n", game, EdgeTable()), "isn't available"));
}

TEST(RunApp, EndOfInputMidHandStandsAutomatically) {
    Game game = riggedGame({10, 10, 8, 9});
    EXPECT_TRUE(contains(runScripted("10\n", game, EdgeTable()), "Dealer wins."));
}

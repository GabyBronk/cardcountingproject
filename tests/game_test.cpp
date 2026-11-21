#include <gtest/gtest.h>
#include "game.h"

namespace {
// Cards are dealt in this order: player, dealer up, player, dealer hole, then draws.
Shoe riggedShoe(const std::vector<int>& ranks) {
    std::vector<Card> cards;
    for (int rank : ranks) cards.push_back(Card{rank, Suit::Clubs});
    return Shoe(cards);
}

Game riggedGame(const std::vector<int>& ranks, double bankroll = 1000.0) {
    return Game(riggedShoe(ranks), bankroll);
}

Game startedGame(const std::vector<int>& ranks, double bet = 10.0) {
    Game game = riggedGame(ranks);
    game.startRound(bet);
    return game;
}

Game gameNearCutCard() {
    Shoe shoe(1, 7);
    for (int i = 0; i < 40; ++i) shoe.discard(shoe.draw());
    return Game(shoe, 1000.0);
}
}  // namespace

// ---------- betting rules ----------
TEST(GameBetting, NewGameIsInBettingPhase) { EXPECT_EQ(Phase::Betting, riggedGame({}).phase()); }
TEST(GameBetting, BetBelowMinimumIsInvalid) { EXPECT_FALSE(riggedGame({}).isValidBet(5.0)); }
TEST(GameBetting, MinimumBetIsValid) { EXPECT_TRUE(riggedGame({}).isValidBet(10.0)); }
TEST(GameBetting, NegativeBetIsInvalid) { EXPECT_FALSE(riggedGame({}).isValidBet(-20.0)); }
TEST(GameBetting, BetAboveBankrollIsInvalid) { EXPECT_FALSE(riggedGame({}, 100.0).isValidBet(150.0)); }
TEST(GameBetting, BetEqualToBankrollIsValid) { EXPECT_TRUE(riggedGame({}, 100.0).isValidBet(100.0)); }

TEST(GameBetting, InvalidBetDoesNotStartRound) {
    Game game = riggedGame({10, 9, 8, 7});
    EXPECT_FALSE(game.startRound(1.0));
}

TEST(GameBetting, InvalidBetDealsNoCards) {
    Game game = riggedGame({10, 9, 8, 7});
    game.startRound(1.0);
    EXPECT_TRUE(game.player().cards().empty());
}

TEST(GameBetting, CannotStartRoundWhileOneIsInProgress) {
    Game game = startedGame({10, 9, 8, 7});
    EXPECT_FALSE(game.startRound(10.0));
}

// ---------- dealing ----------
TEST(GameDeal, PlayerGetsTwoCards) { EXPECT_EQ(2u, startedGame({10, 9, 8, 7}).player().cards().size()); }
TEST(GameDeal, DealerGetsTwoCards) { EXPECT_EQ(2u, startedGame({10, 9, 8, 7}).dealer().cards().size()); }
TEST(GameDeal, PlayerGetsFirstAndThirdCard) { EXPECT_EQ(18, startedGame({10, 9, 8, 7}).player().total()); }
TEST(GameDeal, DealerGetsSecondAndFourthCard) { EXPECT_EQ(16, startedGame({10, 9, 8, 7}).dealer().total()); }
TEST(GameDeal, RoundIsNowPlayerTurn) { EXPECT_EQ(Phase::PlayerTurn, startedGame({10, 9, 8, 7}).phase()); }
TEST(GameDeal, HoleCardStartsHidden) { EXPECT_FALSE(startedGame({10, 9, 8, 7}).holeRevealed()); }
TEST(GameDeal, DealtCardsLeaveTheShoe) { EXPECT_EQ(1, startedGame({10, 9, 8, 7, 5}).cardsRemaining()); }

// ---------- player actions ----------
TEST(GameActions, HitBeforeRoundStartsFails) { EXPECT_FALSE(riggedGame({10, 9, 8, 7}).hit()); }
TEST(GameActions, StandBeforeRoundStartsFails) { EXPECT_FALSE(riggedGame({10, 9, 8, 7}).stand()); }
TEST(GameActions, DoubleBeforeRoundStartsFails) { EXPECT_FALSE(riggedGame({10, 9, 8, 7}).doubleDown()); }

TEST(GameActions, HitAddsOneCardToPlayer) {
    Game game = startedGame({2, 9, 3, 7, 4});
    game.hit();
    EXPECT_EQ(3u, game.player().cards().size());
}

TEST(GameActions, HitWithoutBustKeepsPlayerTurn) {
    Game game = startedGame({2, 9, 3, 7, 4});
    game.hit();
    EXPECT_EQ(Phase::PlayerTurn, game.phase());
}

TEST(GameActions, HittingTo21EndsPlayerTurn) {
    Game game = startedGame({5, 10, 6, 7, 10});
    game.hit();
    EXPECT_EQ(Phase::RoundOver, game.phase());
}

TEST(GameActions, ActionsFailAfterRoundIsOver) {
    Game game = startedGame({10, 10, 8, 9});
    game.stand();
    EXPECT_FALSE(game.hit());
}

// ---------- bust ----------
TEST(GameBust, BustMeansDealerWins) {
    Game game = startedGame({10, 9, 6, 7, 10});
    game.hit();
    EXPECT_EQ(Outcome::DealerWin, game.outcome());
}

TEST(GameBust, BustLosesTheBet) {
    Game game = startedGame({10, 9, 6, 7, 10});
    game.hit();
    EXPECT_DOUBLE_EQ(990.0, game.bankroll());
}

TEST(GameBust, DealerDoesNotDrawAfterPlayerBusts) {
    Game game = startedGame({10, 9, 6, 5, 10, 10});
    game.hit();
    EXPECT_EQ(2u, game.dealer().cards().size());
}

// ---------- dealer play and results ----------
TEST(GameDealer, DealerDrawsUpTo17) {
    Game game = startedGame({10, 10, 7, 5, 2, 10});
    game.stand();
    EXPECT_EQ(17, game.dealer().total());
}

TEST(GameDealer, DealerStandsOnSoft17) {
    Game game = startedGame({10, 1, 9, 6, 5});
    game.stand();
    EXPECT_EQ(2u, game.dealer().cards().size());
}

TEST(GameDealer, DealerDoesNotDrawOnHard17) {
    Game game = startedGame({10, 10, 8, 7, 5});
    game.stand();
    EXPECT_EQ(2u, game.dealer().cards().size());
}

TEST(GameDealer, HoleCardIsRevealedAfterStanding) {
    Game game = startedGame({10, 10, 8, 9});
    game.stand();
    EXPECT_TRUE(game.holeRevealed());
}

TEST(GameResult, DealerBustMeansPlayerWins) {
    Game game = startedGame({10, 10, 8, 6, 10});
    game.stand();
    EXPECT_EQ(Outcome::PlayerWin, game.outcome());
}

TEST(GameResult, WinPaysOneToOne) {
    Game game = startedGame({10, 10, 8, 6, 10});
    game.stand();
    EXPECT_DOUBLE_EQ(1010.0, game.bankroll());
}

TEST(GameResult, HigherDealerTotalWins) {
    Game game = startedGame({10, 10, 8, 9});
    game.stand();
    EXPECT_EQ(Outcome::DealerWin, game.outcome());
}

TEST(GameResult, LossTakesTheBet) {
    Game game = startedGame({10, 10, 8, 9});
    game.stand();
    EXPECT_DOUBLE_EQ(990.0, game.bankroll());
}

TEST(GameResult, EqualTotalsPush) {
    Game game = startedGame({10, 10, 9, 9});
    game.stand();
    EXPECT_EQ(Outcome::Push, game.outcome());
}

TEST(GameResult, PushReturnsTheBet) {
    Game game = startedGame({10, 10, 9, 9});
    game.stand();
    EXPECT_DOUBLE_EQ(1000.0, game.bankroll());
}

// ---------- blackjack ----------
TEST(GameBlackjack, PlayerBlackjackEndsRoundImmediately) {
    EXPECT_EQ(Phase::RoundOver, startedGame({1, 9, 13, 7}).phase());
}

TEST(GameBlackjack, PlayerBlackjackIsTheOutcome) {
    EXPECT_EQ(Outcome::PlayerBlackjack, startedGame({1, 9, 13, 7}).outcome());
}

TEST(GameBlackjack, PlayerBlackjackPaysThreeToTwo) {
    EXPECT_DOUBLE_EQ(1015.0, startedGame({1, 9, 13, 7}).bankroll());
}

TEST(GameBlackjack, DealerBlackjackWinsImmediately) {
    EXPECT_EQ(Outcome::DealerWin, startedGame({10, 1, 9, 13}).outcome());
}

TEST(GameBlackjack, BothBlackjacksPush) {
    EXPECT_EQ(Outcome::Push, startedGame({1, 1, 13, 13}).outcome());
}

// ---------- doubling ----------
TEST(GameDouble, DoubleIsAllowedOnFirstTwoCards) { EXPECT_TRUE(startedGame({5, 10, 6, 7, 10}).canDouble()); }

TEST(GameDouble, DoubleIsNotAllowedAfterAHit) {
    Game game = startedGame({2, 10, 3, 7, 4});
    game.hit();
    EXPECT_FALSE(game.canDouble());
}

TEST(GameDouble, DoubleIsNotAllowedWithoutEnoughBankroll) {
    Game game = riggedGame({5, 10, 6, 7, 10}, 15.0);
    game.startRound(10.0);
    EXPECT_FALSE(game.canDouble());
}

TEST(GameDouble, DoubleDoublesTheBet) {
    Game game = startedGame({5, 10, 6, 7, 10});
    game.doubleDown();
    EXPECT_DOUBLE_EQ(20.0, game.bet());
}

TEST(GameDouble, DoubleDrawsExactlyOneCard) {
    Game game = startedGame({5, 10, 6, 7, 10, 10});
    game.doubleDown();
    EXPECT_EQ(3u, game.player().cards().size());
}

TEST(GameDouble, DoubleEndsTheRound) {
    Game game = startedGame({5, 10, 6, 7, 10});
    game.doubleDown();
    EXPECT_EQ(Phase::RoundOver, game.phase());
}

TEST(GameDouble, WinningDoublePaysTwiceTheOriginalBet) {
    Game game = startedGame({5, 10, 6, 7, 10});
    game.doubleDown();
    EXPECT_DOUBLE_EQ(1020.0, game.bankroll());
}

TEST(GameDouble, BustingOnDoubleLosesTwiceTheOriginalBet) {
    Game game = startedGame({10, 10, 6, 7, 10});
    game.doubleDown();
    EXPECT_DOUBLE_EQ(980.0, game.bankroll());
}

// ---------- counting ----------
TEST(GameCount, HoleCardIsNotCountedWhilePlaying) {
    EXPECT_EQ(3, startedGame({2, 3, 4, 5, 10}).runningCount());
}

TEST(GameCount, HoleCardIsCountedOnceRevealed) {
    EXPECT_EQ(-3, startedGame({10, 1, 9, 13}).runningCount());
}

TEST(GameCount, DrawnCardsAreCounted) {
    Game game = startedGame({2, 9, 3, 7, 4});
    game.hit();
    EXPECT_EQ(3, game.runningCount());
}

TEST(GameCount, TrueCountUsesCardsRemaining) {
    Game game = startedGame({2, 3, 4, 5, 10});
    EXPECT_DOUBLE_EQ(3 / (1 / 52.0), game.trueCount());
}

// ---------- reshuffling ----------
TEST(GameReshuffle, NoNoticeWhenShoeIsFresh) {
    Game game(Shoe(1, 7), 1000.0);
    game.prepareRound();
    EXPECT_TRUE(game.takeNotices().empty());
}

TEST(GameReshuffle, PastCutCardTriggersNotice) {
    Game game = gameNearCutCard();
    game.prepareRound();
    EXPECT_EQ(1u, game.takeNotices().size());
}

TEST(GameReshuffle, PastCutCardRestoresFullShoe) {
    Game game = gameNearCutCard();
    game.prepareRound();
    EXPECT_EQ(52, game.cardsRemaining());
}

TEST(GameReshuffle, TakeNoticesClearsThem) {
    Game game = gameNearCutCard();
    game.prepareRound();
    game.takeNotices();
    EXPECT_TRUE(game.takeNotices().empty());
}

TEST(GameReshuffle, CountResetsToZeroAfterCutCardReshuffle) {
    Game game = gameNearCutCard();
    game.prepareRound();
    EXPECT_EQ(0, game.runningCount());
}

TEST(GameReshuffle, PrepareRoundClearsPlayerHand) {
    Game game = startedGame({10, 10, 8, 9, 5, 5, 5, 5});
    game.stand();
    game.prepareRound();
    EXPECT_TRUE(game.player().cards().empty());
}

TEST(GameReshuffle, PrepareRoundAllowsNextBet) {
    Game game = startedGame({10, 10, 8, 9, 5, 5, 5, 5});
    game.stand();
    game.prepareRound();
    EXPECT_TRUE(game.startRound(10.0));
}

namespace {
Game gameWithEmptyShoeButDiscards() {
    Shoe shoe(std::vector<Card>{{10, Suit::Clubs}, {10, Suit::Clubs}, {6, Suit::Clubs}, {6, Suit::Clubs}});
    for (int i = 0; i < 3; ++i) shoe.discard(Card{2, Suit::Hearts});
    Game game(shoe, 1000.0);
    game.startRound(10.0);
    return game;
}
}  // namespace

TEST(GameReshuffle, EmptyShoeMidRoundTriggersNotice) {
    Game game = gameWithEmptyShoeButDiscards();
    game.hit();
    EXPECT_EQ(1u, game.takeNotices().size());
}

TEST(GameReshuffle, EmptyShoeMidRoundStillDrawsACard) {
    Game game = gameWithEmptyShoeButDiscards();
    game.hit();
    EXPECT_EQ(3u, game.player().cards().size());
}

TEST(GameReshuffle, MidRoundReshuffleKeepsFaceUpCardsInTheCount) {
    Game game = gameWithEmptyShoeButDiscards();
    game.hit();  // table: 10, 6, dealer 10 up (-1) + drawn 2 (+1)
    EXPECT_EQ(0, game.runningCount());
}

TEST(GameReshuffle, NoCardsAnywhereThrows) {
    Game game = startedGame({10, 10, 6, 6});
    EXPECT_THROW(game.hit(), std::runtime_error);
}

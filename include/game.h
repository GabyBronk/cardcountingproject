#pragma once
#include <string>
#include <vector>
#include "counter.h"
#include "hand.h"
#include "shoe.h"

enum class Phase { Betting, PlayerTurn, RoundOver };
enum class Outcome { None, PlayerBlackjack, PlayerWin, DealerWin, Push };

// One player vs. the dealer (dealer stands on all 17s, blackjack pays 3:2).
// Call prepareRound() before each round, then startRound(bet).
class Game {
public:
    static constexpr double kMinBet = 10.0;

    Game(Shoe shoe, double bankroll);

    void prepareRound();             // discard last round's cards; reshuffle if past the cut card
    bool isValidBet(double bet) const;
    bool startRound(double bet);
    bool hit();
    bool stand();
    bool doubleDown();
    bool canDouble() const;

    Phase phase() const;
    Outcome outcome() const;
    double bankroll() const;
    double bet() const;
    const Hand& player() const;
    const Hand& dealer() const;
    bool holeRevealed() const;

    int runningCount() const;
    double trueCount() const;
    int cardsRemaining() const;
    int numDecks() const;
    std::vector<std::string> takeNotices();

private:
    Card drawCard();
    void dealToPlayer();
    void dealToDealer(bool faceUp);
    void dealOpeningHands();
    void dealerTurn();
    void finishRound();
    void revealHole();
    Outcome decideOutcome() const;
    Outcome compareTotals() const;
    double payout() const;
    void discardHand(Hand& hand);
    void reshuffleShoe(const std::string& reason);
    void recountTable();
    std::vector<Card> visibleDealerCards() const;

    Shoe shoe_;
    Counter counter_;
    Hand player_;
    Hand dealer_;
    std::vector<std::string> notices_;
    double bankroll_;
    double bet_ = 0.0;
    Phase phase_ = Phase::Betting;
    Outcome outcome_ = Outcome::None;
    bool holeRevealed_ = false;
};

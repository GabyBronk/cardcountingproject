#include "game.h"
#include <utility>

static const int kBlackjack = 21;
static const int kDealerStandsOn = 17;

Game::Game(Shoe shoe, double bankroll)
    : shoe_(std::move(shoe)), bankroll_(bankroll) {}

// ---------- round setup ----------

void Game::discardHand(Hand& hand) {
    for (const Card& card : hand.cards()) shoe_.discard(card);
    hand.clear();
}

void Game::prepareRound() {
    discardHand(player_);
    discardHand(dealer_);
    holeRevealed_ = false;
    if (shoe_.needsReshuffle()) reshuffleShoe("Cut card reached");
    phase_ = Phase::Betting;
}

bool Game::isValidBet(double bet) const {
    return bet >= kMinBet && bet <= bankroll_;
}

bool Game::startRound(double bet) {
    if (phase_ != Phase::Betting || !isValidBet(bet)) return false;
    bet_ = bet;
    outcome_ = Outcome::None;
    dealOpeningHands();
    return true;
}

void Game::dealOpeningHands() {
    phase_ = Phase::PlayerTurn;
    dealToPlayer();
    dealToDealer(true);
    dealToPlayer();
    dealToDealer(false);  // hole card: drawn but not counted until revealed
    if (player_.isBlackjack() || dealer_.isBlackjack()) finishRound();
}

// ---------- drawing and counting ----------

Card Game::drawCard() {
    if (shoe_.isEmpty()) reshuffleShoe("Shoe is empty");
    return shoe_.draw();
}

void Game::dealToPlayer() {
    Card card = drawCard();
    player_.add(card);
    counter_.see(card);
}

void Game::dealToDealer(bool faceUp) {
    Card card = drawCard();
    dealer_.add(card);
    if (faceUp) counter_.see(card);
}

void Game::revealHole() {
    if (holeRevealed_ || dealer_.cards().size() < 2) return;
    counter_.see(dealer_.cards()[1]);
    holeRevealed_ = true;
}

std::vector<Card> Game::visibleDealerCards() const {
    std::vector<Card> cards = dealer_.cards();
    if (!holeRevealed_ && cards.size() > 1) cards.erase(cards.begin() + 1);
    return cards;
}

// Cards still on the table are out of the shoe, so after a reshuffle the
// count must start from those face-up cards instead of from zero.
void Game::recountTable() {
    counter_.reset();
    for (const Card& card : player_.cards()) counter_.see(card);
    for (const Card& card : visibleDealerCards()) counter_.see(card);
}

void Game::reshuffleShoe(const std::string& reason) {
    notices_.push_back(reason + " - reshuffling the shoe. Count reset.");
    shoe_.reshuffle();
    recountTable();
}

// ---------- player actions ----------

bool Game::hit() {
    if (phase_ != Phase::PlayerTurn) return false;
    dealToPlayer();
    if (player_.isBust()) finishRound();
    else if (player_.total() == kBlackjack) dealerTurn();
    return true;
}

bool Game::stand() {
    if (phase_ != Phase::PlayerTurn) return false;
    dealerTurn();
    return true;
}

bool Game::canDouble() const {
    return phase_ == Phase::PlayerTurn && player_.cards().size() == 2 &&
           bankroll_ >= bet_ * 2;
}

bool Game::doubleDown() {
    if (!canDouble()) return false;
    bet_ *= 2;
    dealToPlayer();
    if (player_.isBust()) finishRound();
    else dealerTurn();
    return true;
}

// ---------- dealer and payout ----------

void Game::dealerTurn() {
    revealHole();
    while (dealer_.total() < kDealerStandsOn) dealToDealer(true);
    finishRound();
}

void Game::finishRound() {
    revealHole();
    outcome_ = decideOutcome();
    bankroll_ += payout();
    phase_ = Phase::RoundOver;
}

Outcome Game::compareTotals() const {
    if (player_.total() > dealer_.total()) return Outcome::PlayerWin;
    if (player_.total() < dealer_.total()) return Outcome::DealerWin;
    return Outcome::Push;
}

Outcome Game::decideOutcome() const {
    if (player_.isBust()) return Outcome::DealerWin;
    if (player_.isBlackjack() && !dealer_.isBlackjack()) return Outcome::PlayerBlackjack;
    if (dealer_.isBust()) return Outcome::PlayerWin;
    return compareTotals();
}

double Game::payout() const {
    switch (outcome_) {
        case Outcome::PlayerBlackjack: return bet_ * 1.5;
        case Outcome::PlayerWin: return bet_;
        case Outcome::DealerWin: return -bet_;
        default: return 0.0;
    }
}

// ---------- read-only views ----------

Phase Game::phase() const { return phase_; }
Outcome Game::outcome() const { return outcome_; }
double Game::bankroll() const { return bankroll_; }
double Game::bet() const { return bet_; }
const Hand& Game::player() const { return player_; }
const Hand& Game::dealer() const { return dealer_; }
bool Game::holeRevealed() const { return holeRevealed_; }
int Game::runningCount() const { return counter_.runningCount(); }
int Game::cardsRemaining() const { return shoe_.cardsRemaining(); }
int Game::numDecks() const { return shoe_.numDecks(); }

double Game::trueCount() const {
    return counter_.trueCount(shoe_.cardsRemaining());
}

std::vector<std::string> Game::takeNotices() {
    std::vector<std::string> taken;
    taken.swap(notices_);
    return taken;
}

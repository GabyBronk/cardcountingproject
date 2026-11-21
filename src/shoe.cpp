#include "shoe.h"
#include <algorithm>
#include <stdexcept>

static const double kCutCardFraction = 0.25;  // reshuffle when under 25% remains

Shoe::Shoe(int numDecks, unsigned seed)
    : rng_(seed), totalCards_(numDecks * 52) {
    std::vector<Card> cards = buildDecks(numDecks);
    shuffleAndLoad(cards);
}

Shoe::Shoe(const std::vector<Card>& orderedCards)
    : rng_(0), totalCards_(static_cast<int>(orderedCards.size())) {
    for (const Card& card : orderedCards) shoe_.push(card);
}

void Shoe::shuffleAndLoad(std::vector<Card>& cards) {
    std::shuffle(cards.begin(), cards.end(), rng_);
    for (const Card& card : cards) shoe_.push(card);
    cards.clear();
}

Card Shoe::draw() {
    if (shoe_.empty()) throw std::runtime_error("Cannot draw from an empty shoe");
    Card top = shoe_.front();
    shoe_.pop();
    return top;
}

void Shoe::discard(const Card& card) { discards_.push_back(card); }

void Shoe::reshuffle() {
    while (!shoe_.empty()) {
        discards_.push_back(shoe_.front());
        shoe_.pop();
    }
    shuffleAndLoad(discards_);
}

bool Shoe::isEmpty() const { return shoe_.empty(); }
int Shoe::cardsRemaining() const { return static_cast<int>(shoe_.size()); }
int Shoe::discardCount() const { return static_cast<int>(discards_.size()); }
int Shoe::numDecks() const { return totalCards_ / 52; }

bool Shoe::needsReshuffle() const {
    return cardsRemaining() < totalCards_ * kCutCardFraction;
}

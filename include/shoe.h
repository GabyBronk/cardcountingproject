#pragma once
#include <queue>
#include <random>
#include <vector>
#include "card.h"

// The shoe is a queue (draw pile). Shuffling happens in a vector first,
// then the shuffled cards are loaded into the queue.
class Shoe {
public:
    Shoe(int numDecks, unsigned seed);
    explicit Shoe(const std::vector<Card>& orderedCards);  // no shuffle; used by tests

    Card draw();                      // throws std::runtime_error if empty
    void discard(const Card& card);   // goes to the discard vector
    void reshuffle();                 // shoe + discards -> shuffled -> shoe

    bool isEmpty() const;
    bool needsReshuffle() const;      // true once the cut card (75% penetration) is passed
    int cardsRemaining() const;
    int discardCount() const;
    int numDecks() const;

private:
    void shuffleAndLoad(std::vector<Card>& cards);

    std::queue<Card> shoe_;
    std::vector<Card> discards_;
    std::mt19937 rng_;
    int totalCards_;
};

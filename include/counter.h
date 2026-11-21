#pragma once
#include "card.h"

double decksRemaining(int cardsRemaining);

// Hi-Lo running count of every face-up card seen since the last shuffle.
class Counter {
public:
    void see(const Card& card);
    void reset();
    int runningCount() const;
    double trueCount(int cardsRemaining) const;  // running count / decks remaining

private:
    int running_ = 0;
};

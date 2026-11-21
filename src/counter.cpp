#include "counter.h"

double decksRemaining(int cardsRemaining) { return cardsRemaining / 52.0; }

void Counter::see(const Card& card) { running_ += hiLoValue(card); }
void Counter::reset() { running_ = 0; }
int Counter::runningCount() const { return running_; }

double Counter::trueCount(int cardsRemaining) const {
    if (cardsRemaining <= 0) return 0.0;
    return running_ / decksRemaining(cardsRemaining);
}

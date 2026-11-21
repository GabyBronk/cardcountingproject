#include "hand.h"
#include <algorithm>

void Hand::add(const Card& card) { cards_.push_back(card); }
void Hand::clear() { cards_.clear(); }
const std::vector<Card>& Hand::cards() const { return cards_; }

int Hand::rawSum() const {
    int sum = 0;
    for (const Card& card : cards_) sum += blackjackValue(card);
    return sum;
}

int Hand::aceCount() const {
    return static_cast<int>(std::count_if(cards_.begin(), cards_.end(),
        [](const Card& card) { return card.rank == 1; }));
}

int Hand::hardTotal() const { return rawSum() - 10 * aceCount(); }

bool Hand::isSoft() const { return aceCount() > 0 && hardTotal() + 10 <= 21; }
int Hand::total() const { return hardTotal() + (isSoft() ? 10 : 0); }
bool Hand::isBust() const { return total() > 21; }
bool Hand::isBlackjack() const { return cards_.size() == 2 && total() == 21; }

std::string Hand::toString() const {
    std::string text;
    for (const Card& card : cards_) text += cardToString(card) + " ";
    if (!text.empty()) text.pop_back();
    return text;
}

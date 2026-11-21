#pragma once
#include <string>
#include <vector>
#include "card.h"

class Hand {
public:
    void add(const Card& card);
    void clear();
    const std::vector<Card>& cards() const;

    int total() const;        // best total; an Ace counts 11 when that doesn't bust
    bool isSoft() const;
    bool isBust() const;
    bool isBlackjack() const; // exactly two cards totalling 21
    std::string toString() const;

private:
    int rawSum() const;
    int aceCount() const;
    int hardTotal() const;

    std::vector<Card> cards_;
};

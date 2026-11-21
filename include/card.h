#pragma once
#include <string>
#include <vector>

enum class Suit { Clubs, Diamonds, Hearts, Spades };

struct Card {
    int rank;  // 1 = Ace, 2-10 = number cards, 11 = J, 12 = Q, 13 = K
    Suit suit;
};

bool operator==(const Card& a, const Card& b);

bool isValidDeckCount(int numDecks);
int blackjackValue(const Card& card);  // Ace is 11 here; Hand handles soft totals
int hiLoValue(const Card& card);
std::string cardToString(const Card& card);
std::vector<Card> buildDecks(int numDecks);

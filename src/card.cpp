#include "card.h"
#include <algorithm>

bool operator==(const Card& a, const Card& b) {
    return a.rank == b.rank && a.suit == b.suit;
}

bool isValidDeckCount(int numDecks) {
    return numDecks == 1 || numDecks == 2 || numDecks == 4;
}

int blackjackValue(const Card& card) {
    if (card.rank == 1) return 11;
    return std::min(card.rank, 10);
}

int hiLoValue(const Card& card) {
    if (card.rank >= 2 && card.rank <= 6) return 1;
    if (card.rank >= 7 && card.rank <= 9) return 0;
    return -1;
}

static std::string rankToString(int rank) {
    static const char* names[] = {"A", "2", "3", "4", "5", "6", "7",
                                  "8", "9", "10", "J", "Q", "K"};
    if (rank < 1 || rank > 13) return "?";
    return names[rank - 1];
}

static std::string suitToString(Suit suit) {
    static const char* names[] = {"C", "D", "H", "S"};
    return names[static_cast<int>(suit)];
}

std::string cardToString(const Card& card) {
    return rankToString(card.rank) + suitToString(card.suit);
}

std::vector<Card> buildDecks(int numDecks) {
    std::vector<Card> cards;
    for (int deck = 0; deck < numDecks; ++deck)
        for (int suit = 0; suit < 4; ++suit)
            for (int rank = 1; rank <= 13; ++rank)
                cards.push_back(Card{rank, static_cast<Suit>(suit)});
    return cards;
}

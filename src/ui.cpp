#include "ui.h"
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include "betting.h"

namespace {

const double kStartingBankroll = 1000.0;

std::string trim(const std::string& text) {
    size_t first = text.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

std::string formatNumber(double value, int decimals, bool showSign) {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(decimals);
    if (showSign && value > 0) stream << "+";
    stream << value;
    return stream.str();
}

std::string money(double value) { return "$" + formatNumber(value, 2, false); }

Action actionFromLetter(int letter) {
    switch (letter) {
        case 'h': return Action::Hit;
        case 's': return Action::Stand;
        case 'd': return Action::Double;
        case 'q': return Action::Quit;
        default: return Action::Unknown;
    }
}

// ---------- printing ----------

void printCountLine(std::ostream& out, const Game& game) {
    out << "Shoe: " << game.cardsRemaining() << " cards | Running count: "
        << formatNumber(game.runningCount(), 0, true) << " | True count: "
        << formatNumber(game.trueCount(), 1, true) << "\n";
}

void printEdgeLine(std::ostream& out, const Game& game, const EdgeTable& table) {
    std::optional<double> edge = table.edgeFor(game.numDecks(), game.trueCount());
    if (!edge) { out << "Estimated edge: n/a\n"; return; }
    double suggested = recommendedUnits(*edge) * Game::kMinBet;
    out << "Estimated edge: " << formatNumber(*edge, 2, true) << "% | Suggested bet: "
        << money(std::min(suggested, game.bankroll())) << "\n";
}

void printDealer(std::ostream& out, const Game& game) {
    const Hand& dealer = game.dealer();
    if (game.holeRevealed()) {
        out << "Dealer: " << dealer.toString() << " (" << dealer.total() << ")\n";
    } else {
        out << "Dealer: " << cardToString(dealer.cards()[0]) << " [hidden]\n";
    }
}

void printPlayer(std::ostream& out, const Game& game) {
    const Hand& hand = game.player();
    out << "You:    " << hand.toString() << " ("
        << (hand.isSoft() ? "soft " : "") << hand.total() << ")\n";
}

std::string outcomeMessage(Outcome outcome) {
    switch (outcome) {
        case Outcome::PlayerBlackjack: return "Blackjack! Pays 3:2.";
        case Outcome::PlayerWin: return "You win!";
        case Outcome::DealerWin: return "Dealer wins.";
        case Outcome::Push: return "Push - bet returned.";
        default: return "";
    }
}

void printResult(std::ostream& out, const Game& game) {
    printDealer(out, game);
    printPlayer(out, game);
    out << outcomeMessage(game.outcome()) << "\n";
    out << "Bankroll: " << money(game.bankroll()) << "\n\n";
}

void printNotices(std::ostream& out, Game& game) {
    for (const std::string& notice : game.takeNotices()) out << "*** " << notice << " ***\n";
}

void printWelcome(std::ostream& out) {
    out << "=== Card Counting Simulator ===\n"
        << "Hi-Lo: 2-6 = +1, 7-9 = 0, 10/J/Q/K/A = -1.\n"
        << "True count = running count / decks remaining.\n"
        << "Raise your bet when the edge is positive. Dealer stands on all 17s.\n\n";
}

// ---------- input ----------

std::optional<double> validBet(const std::string& line, const Game& game) {
    std::optional<double> bet = parseNumber(line);
    if (bet && game.isValidBet(*bet)) return bet;
    return std::nullopt;
}

std::optional<double> askBet(std::istream& in, std::ostream& out, const Game& game,
                             const EdgeTable& table) {
    printBettingInfo(out, game, table);
    std::string line;
    out << "Bet (min $10, 'q' quits): ";
    while (std::getline(in, line)) {
        if (parseAction(line) == Action::Quit) return std::nullopt;
        if (std::optional<double> bet = validBet(line, game)) return bet;
        out << "Invalid bet. Try again: ";
    }
    return std::nullopt;
}

bool performAction(Game& game, Action action) {
    switch (action) {
        case Action::Hit: return game.hit();
        case Action::Stand: return game.stand();
        case Action::Double: return game.doubleDown();
        default: return false;
    }
}

void askAction(std::istream& in, std::ostream& out, Game& game) {
    std::string line;
    out << "(h)it, (s)tand" << (game.canDouble() ? ", (d)ouble" : "") << ": ";
    if (!std::getline(in, line)) { game.stand(); return; }
    if (!performAction(game, parseAction(line))) out << "That action isn't available.\n";
}

void playerTurns(std::istream& in, std::ostream& out, Game& game) {
    while (game.phase() == Phase::PlayerTurn) {
        printNotices(out, game);
        printDealer(out, game);
        printPlayer(out, game);
        printCountLine(out, game);
        askAction(in, out, game);
    }
}

bool placeBet(std::istream& in, std::ostream& out, Game& game, const EdgeTable& table) {
    game.prepareRound();
    printNotices(out, game);
    std::optional<double> bet = askBet(in, out, game, table);
    return bet && game.startRound(*bet);
}

}  // namespace

// ---------- public API ----------

Action parseAction(const std::string& input) {
    std::string text = trim(input);
    if (text.empty()) return Action::Unknown;
    return actionFromLetter(std::tolower(static_cast<unsigned char>(text[0])));
}

std::optional<double> parseNumber(const std::string& input) {
    std::string text = trim(input);
    size_t used = 0;
    try {
        double value = std::stod(text, &used);
        if (used == text.size()) return value;
    } catch (const std::exception&) {}
    return std::nullopt;
}

std::optional<int> parseWholeNumber(const std::string& input) {
    std::string text = trim(input);
    size_t used = 0;
    try {
        int value = std::stoi(text, &used);
        if (used == text.size()) return value;
    } catch (const std::exception&) {}
    return std::nullopt;
}

void printBettingInfo(std::ostream& out, const Game& game, const EdgeTable& table) {
    out << "Bankroll: " << money(game.bankroll()) << "\n";
    printCountLine(out, game);
    printEdgeLine(out, game, table);
}

bool playRound(std::istream& in, std::ostream& out, Game& game, const EdgeTable& table) {
    if (!placeBet(in, out, game, table)) return false;
    playerTurns(in, out, game);
    printNotices(out, game);
    printResult(out, game);
    return true;
}

void runApp(std::istream& in, std::ostream& out, Game& game, const EdgeTable& table) {
    printWelcome(out);
    while (game.bankroll() >= Game::kMinBet && playRound(in, out, game, table)) {}
    out << "Final bankroll: " << money(game.bankroll()) << "\n";
}

int askDeckCount(std::istream& in, std::ostream& out) {
    std::string line;
    out << "Number of decks in the shoe (1, 2, or 4): ";
    while (std::getline(in, line)) {
        std::optional<int> decks = parseWholeNumber(line);
        if (decks && isValidDeckCount(*decks)) return *decks;
        out << "Please enter 1, 2, or 4: ";
    }
    return 0;
}

int runProgram(std::istream& in, std::ostream& out, const std::string& csvPath) {
    EdgeTable table;
    if (!table.loadFromFile(csvPath)) { out << "Could not open " << csvPath << "\n"; return 1; }
    int decks = askDeckCount(in, out);
    if (decks == 0) return 0;
    Game game(Shoe(decks, std::random_device{}()), kStartingBankroll);
    runApp(in, out, game, table);
    return 0;
}

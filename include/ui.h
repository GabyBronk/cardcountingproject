#pragma once
#include <iosfwd>
#include <optional>
#include <string>
#include "edge_table.h"
#include "game.h"

enum class Action { Hit, Stand, Double, Quit, Unknown };

Action parseAction(const std::string& input);
std::optional<double> parseNumber(const std::string& input);
std::optional<int> parseWholeNumber(const std::string& input);

void printBettingInfo(std::ostream& out, const Game& game, const EdgeTable& table);
bool playRound(std::istream& in, std::ostream& out, Game& game, const EdgeTable& table);
void runApp(std::istream& in, std::ostream& out, Game& game, const EdgeTable& table);
int askDeckCount(std::istream& in, std::ostream& out);
int runProgram(std::istream& in, std::ostream& out, const std::string& csvPath);

#include "edge_table.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>

static std::vector<std::string> splitCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::stringstream stream(line);
    std::string field;
    while (std::getline(stream, field, ',')) fields.push_back(field);
    return fields;
}

std::optional<EdgeRow> parseEdgeRow(const std::string& line) {
    std::vector<std::string> fields = splitCsvLine(line);
    if (fields.size() != 3) return std::nullopt;
    try {
        return EdgeRow{std::stoi(fields[0]), std::stoi(fields[1]), std::stod(fields[2])};
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

static int clampTrueCount(double trueCount) {
    long rounded = std::lround(trueCount);
    return static_cast<int>(std::clamp<long>(rounded, EdgeTable::kMinTrueCount,
                                             EdgeTable::kMaxTrueCount));
}

const EdgeRow* EdgeTable::findRow(int decks, int trueCount) const {
    for (const EdgeRow& row : rows_)
        if (row.decks == decks && row.trueCount == trueCount) return &row;
    return nullptr;
}

bool EdgeTable::addRow(const EdgeRow& row) {
    if (findRow(row.decks, row.trueCount) != nullptr) return false;
    rows_.push_back(row);
    return true;
}

void EdgeTable::addLine(const std::string& line) {
    std::optional<EdgeRow> row = parseEdgeRow(line);
    if (row) addRow(*row);
}

bool EdgeTable::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return false;
    std::string line;
    std::getline(file, line);  // header row
    while (std::getline(file, line)) addLine(line);
    return true;
}

std::optional<double> EdgeTable::edgeFor(int decks, double trueCount) const {
    const EdgeRow* row = findRow(decks, clampTrueCount(trueCount));
    if (row == nullptr) return std::nullopt;
    return row->edgePercent;
}

int EdgeTable::size() const { return static_cast<int>(rows_.size()); }

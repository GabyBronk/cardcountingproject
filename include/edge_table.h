#pragma once
#include <optional>
#include <string>
#include <vector>

// One CSV row: the player's estimated edge (in percent) for a deck count and true count.
struct EdgeRow {
    int decks;
    int trueCount;
    double edgePercent;
};

std::optional<EdgeRow> parseEdgeRow(const std::string& line);

class EdgeTable {
public:
    static constexpr int kMinTrueCount = -10;
    static constexpr int kMaxTrueCount = 10;

    bool addRow(const EdgeRow& row);  // false if (decks, trueCount) already exists
    bool loadFromFile(const std::string& path);  // false if the file can't be opened
    std::optional<double> edgeFor(int decks, double trueCount) const;
    int size() const;

private:
    const EdgeRow* findRow(int decks, int trueCount) const;
    void addLine(const std::string& line);

    std::vector<EdgeRow> rows_;
};

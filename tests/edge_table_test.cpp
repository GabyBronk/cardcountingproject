#include <gtest/gtest.h>
#include <cstdio>
#include <fstream>
#include "edge_table.h"

namespace {
const std::string kRealCsv = std::string(DATA_DIR) + "/edge_table.csv";

EdgeTable tableWithRow(int decks, int trueCount, double edge) {
    EdgeTable table;
    table.addRow(EdgeRow{decks, trueCount, edge});
    return table;
}

EdgeTable loadedRealTable() {
    EdgeTable table;
    table.loadFromFile(kRealCsv);
    return table;
}

EdgeTable loadFromText(const std::string& text) {
    const std::string path = "edge_table_test_tmp.csv";
    std::ofstream(path) << text;
    EdgeTable table;
    table.loadFromFile(path);
    std::remove(path.c_str());
    return table;
}
}  // namespace

TEST(ParseEdgeRow, ReadsDeckCount) { EXPECT_EQ(2, parseEdgeRow("2,3,1.25")->decks); }
TEST(ParseEdgeRow, ReadsTrueCount) { EXPECT_EQ(-4, parseEdgeRow("2,-4,1.25")->trueCount); }
TEST(ParseEdgeRow, ReadsEdgePercent) { EXPECT_DOUBLE_EQ(1.25, parseEdgeRow("2,3,1.25")->edgePercent); }
TEST(ParseEdgeRow, RejectsTooFewFields) { EXPECT_FALSE(parseEdgeRow("2,3").has_value()); }
TEST(ParseEdgeRow, RejectsNonNumericFields) { EXPECT_FALSE(parseEdgeRow("a,b,c").has_value()); }
TEST(ParseEdgeRow, RejectsEmptyLine) { EXPECT_FALSE(parseEdgeRow("").has_value()); }
TEST(ParseEdgeRow, RejectsHeaderLine) { EXPECT_FALSE(parseEdgeRow("decks,true_count,edge_percent").has_value()); }

TEST(EdgeTable, StartsEmpty) { EXPECT_EQ(0, EdgeTable().size()); }

TEST(EdgeTable, AddRowIncreasesSize) {
    EXPECT_EQ(1, tableWithRow(1, 0, -0.2).size());
}

TEST(EdgeTable, AddRowAcceptsNewRow) {
    EdgeTable table;
    EXPECT_TRUE(table.addRow(EdgeRow{1, 0, -0.2}));
}

TEST(EdgeTable, AddRowRejectsDuplicate) {
    EdgeTable table = tableWithRow(1, 0, -0.2);
    EXPECT_FALSE(table.addRow(EdgeRow{1, 0, 9.9}));
}

TEST(EdgeTable, DuplicateKeepsOriginalValue) {
    EdgeTable table = tableWithRow(1, 0, -0.2);
    table.addRow(EdgeRow{1, 0, 9.9});
    EXPECT_DOUBLE_EQ(-0.2, *table.edgeFor(1, 0));
}

TEST(EdgeTable, SameTrueCountOnDifferentDeckCountsIsNotDuplicate) {
    EdgeTable table = tableWithRow(1, 0, -0.2);
    EXPECT_TRUE(table.addRow(EdgeRow{2, 0, -0.4}));
}

TEST(EdgeTable, EdgeForExactRow) { EXPECT_DOUBLE_EQ(1.3, *tableWithRow(1, 3, 1.3).edgeFor(1, 3.0)); }
TEST(EdgeTable, EdgeForRoundsDown) { EXPECT_TRUE(tableWithRow(1, 2, 0.8).edgeFor(1, 2.4).has_value()); }
TEST(EdgeTable, EdgeForRoundsUp) { EXPECT_TRUE(tableWithRow(1, 3, 1.3).edgeFor(1, 2.6).has_value()); }
TEST(EdgeTable, EdgeForRoundsNegativeAwayFromZero) { EXPECT_TRUE(tableWithRow(1, -3, -1.7).edgeFor(1, -2.6).has_value()); }
TEST(EdgeTable, EdgeForClampsHighTrueCounts) { EXPECT_TRUE(tableWithRow(1, 10, 4.8).edgeFor(1, 50.0).has_value()); }
TEST(EdgeTable, EdgeForClampsLowTrueCounts) { EXPECT_TRUE(tableWithRow(1, -10, -5.2).edgeFor(1, -50.0).has_value()); }
TEST(EdgeTable, EdgeForUnknownDeckCountIsEmpty) { EXPECT_FALSE(tableWithRow(1, 0, -0.2).edgeFor(6, 0).has_value()); }
TEST(EdgeTable, EdgeForMissingTrueCountIsEmpty) { EXPECT_FALSE(tableWithRow(1, 0, -0.2).edgeFor(1, 3).has_value()); }
TEST(EdgeTable, EdgeForOnEmptyTableIsEmpty) { EXPECT_FALSE(EdgeTable().edgeFor(1, 0).has_value()); }

TEST(EdgeTable, LoadMissingFileReturnsFalse) {
    EdgeTable table;
    EXPECT_FALSE(table.loadFromFile("no_such_file.csv"));
}

TEST(EdgeTable, LoadRealCsvReturnsTrue) {
    EdgeTable table;
    EXPECT_TRUE(table.loadFromFile(kRealCsv));
}

TEST(EdgeTable, RealCsvHasAtLeast30Rows) { EXPECT_GE(loadedRealTable().size(), 30); }
TEST(EdgeTable, RealCsvHasNegativeEdgeAtTrueCountZero) { EXPECT_LT(*loadedRealTable().edgeFor(1, 0), 0.0); }
TEST(EdgeTable, RealCsvEdgeRisesWithTrueCount) { EXPECT_GT(*loadedRealTable().edgeFor(2, 5), *loadedRealTable().edgeFor(2, 0)); }
TEST(EdgeTable, RealCsvCoversAllThreeDeckCounts) { EXPECT_TRUE(loadedRealTable().edgeFor(4, 0).has_value()); }

TEST(EdgeTable, LoadSkipsHeaderRow) { EXPECT_EQ(1, loadFromText("decks,true_count,edge_percent\n1,0,-0.2\n").size()); }
TEST(EdgeTable, LoadSkipsMalformedLines) { EXPECT_EQ(1, loadFromText("header\n1,0,-0.2\nnot,a,row\n2,3\n").size()); }
TEST(EdgeTable, LoadSkipsBlankLines) { EXPECT_EQ(1, loadFromText("header\n\n1,0,-0.2\n\n").size()); }
TEST(EdgeTable, LoadIgnoresDuplicateRows) { EXPECT_EQ(1, loadFromText("header\n1,0,-0.2\n1,0,5.0\n").size()); }
TEST(EdgeTable, LoadEmptyFileLoadsNothing) { EXPECT_EQ(0, loadFromText("").size()); }
TEST(EdgeTable, LoadHeaderOnlyFileLoadsNothing) { EXPECT_EQ(0, loadFromText("decks,true_count,edge_percent\n").size()); }

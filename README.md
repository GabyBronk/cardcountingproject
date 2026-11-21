# Card Counting Simulator (Hi-Lo Blackjack Trainer)

A console blackjack game that teaches basic card counting. You play against a dealer from a
1-, 2-, or 4-deck shoe while the program shows the **running count**, **true count**, your
estimated **player edge**, and a **suggested bet** (bigger when the edge is higher).

Built in C++17 with Google Test for the Fall 2026 Design Project 2270.

## Requirements
- A C++17 compiler (g++, clang++, or MSVC)
- CMake 3.14 or newer
- Internet access the first time you configure (CMake downloads Google Test automatically)

## Build
```bash
cmake -S . -B build
cmake --build build
```

## Run the application
Run from the repo root so the program can find `data/edge_table.csv`:
```bash
./build/blackjack_sim
```
(On Windows with Visual Studio the executables are in `build/Debug/`.)
You can also pass a different CSV: `./build/blackjack_sim path/to/edge_table.csv`

How to play: choose 1, 2, or 4 decks, type a bet (minimum $10, you start with $1000, `q` quits at
the bet prompt), then type `h` (hit), `s` (stand), or `d` (double, only on your first two cards).
Dealer stands on all 17s. Blackjack pays 3:2.

## Run the tests
```bash
./build/run_tests
```
(or `cd build && ctest`)

## Data structures
| Structure | Used for |
|---|---|
| `std::queue<Card>` | The shoe (draw pile) |
| `std::vector<Card>` | Player/dealer hands, discard pile, and the buffer that is shuffled before cards move into the queue |
| `std::vector<EdgeRow>` | Rows loaded from `data/edge_table.csv` |

## Project layout
```
include/   headers      src/   implementation      tests/   Google Test files
data/edge_table.csv     docs/  specs, reflection, chat history
```
| Module | Responsibility |
|---|---|
| `card` | Card type, blackjack value, Hi-Lo value, building decks |
| `shoe` | Queue-based shoe, shuffle, discard pile, reshuffle, cut card |
| `hand` | Hand total (soft/hard aces), bust, blackjack |
| `counter` | Running count and true count |
| `edge_table` | Loads the CSV, looks up edge by deck count and true count |
| `betting` | Turns an edge into a suggested bet size |
| `game` | Round flow: bet, deal, hit/stand/double, dealer play, payout, reshuffles |
| `ui` | Console input/output |

## Data file
`data/edge_table.csv` has 63 rows: `decks, true_count, edge_percent` for 1, 2, and 4 decks and
true counts -10 to +10. One row = the estimated player edge (in %) at that deck count and true count.
The values come from a rule of thumb (edge = baseline house edge + about 0.5% per true count), not
from a published simulation, so cite or replace them before submitting.

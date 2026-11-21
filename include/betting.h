#pragma once

// Bet ramp: 1 unit when the edge is zero or negative, then +1 unit per
// extra 0.5% of player edge, capped at 12 units.
int recommendedUnits(double edgePercent);

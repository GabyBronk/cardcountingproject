#include "betting.h"
#include <algorithm>

static const double kPercentPerUnit = 0.5;
static const int kMaxUnits = 12;

int recommendedUnits(double edgePercent) {
    if (edgePercent <= 0.0) return 1;
    int units = 1 + static_cast<int>(edgePercent / kPercentPerUnit);
    return std::min(units, kMaxUnits);
}

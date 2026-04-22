#include "Wave.h"
#include <cmath>

double Wave::intensity(double x, double y) const {
    auto p = phasor(x, y);
    return std::norm(p);          // |phasor|^2  (std::norm = squared magnitude)
}

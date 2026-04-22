#include "Screen.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <stdexcept>

Screen::Screen(double screenX, double yMin, double yMax, int numSamples)
    : screenX_(screenX), yMin_(yMin), yMax_(yMax), numSamples_(numSamples) {
    if (numSamples_ < 2)
        throw std::invalid_argument("Screen requires at least 2 sample points.");
}

std::vector<ScreenSample> Screen::measure(const Wave& wave) const {
    std::vector<ScreenSample> result;
    result.reserve(static_cast<std::size_t>(numSamples_));

    double step = (yMax_ - yMin_) / (numSamples_ - 1);
    for (int i = 0; i < numSamples_; ++i) {
        double y = yMin_ + i * step;
        result.push_back({y, wave.intensity(screenX_, y)});
    }
    return result;
}

void Screen::printPattern(const std::vector<ScreenSample>& samples,
                          int barWidth) const {
    if (samples.empty()) return;

    double maxI = std::max_element(samples.begin(), samples.end(),
                    [](const ScreenSample& a, const ScreenSample& b){
                        return a.intensity < b.intensity;
                    })->intensity;

    if (maxI < 1e-30) {
        std::cout << "(all intensities are zero)\n";
        return;
    }

    std::cout << "\n  --- Intensity pattern on screen (x = "
              << screenX_ << ") ---\n\n";

    for (const auto& s : samples) {
        int filled = static_cast<int>(barWidth * s.intensity / maxI);
        std::cout << std::setw(7) << std::fixed << std::setprecision(3)
                  << s.y << " | "
                  << std::string(static_cast<std::size_t>(filled), '#')
                  << "\n";
    }
    std::cout << "\n";
}

void Screen::exportCSV(const std::vector<ScreenSample>& samples,
                       const std::string& filename) const {
    std::ofstream f(filename);
    if (!f) throw std::runtime_error("Cannot open file: " + filename);

    f << "y,intensity\n";
    for (const auto& s : samples) {
        f << std::fixed << std::setprecision(8)
          << s.y << "," << s.intensity << "\n";
    }
    std::cout << "  [Screen] Data exported to: " << filename << "\n";
}

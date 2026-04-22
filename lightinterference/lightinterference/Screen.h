#pragma once
#include "Wave.h"
#include <vector>
#include <string>

// ---------------------------------------------------------------------------
// Screen  –  1-D detector placed at x = screenX
//
// Samples the intensity pattern produced by a Wave (typically a
// CompositeWave) at evenly spaced y-positions between yMin and yMax.
// ---------------------------------------------------------------------------
struct ScreenSample {
    double y;
    double intensity;
};

class Screen {
public:
    Screen(double screenX, double yMin, double yMax, int numSamples);

    // Evaluate the wave's intensity at every sample point
    std::vector<ScreenSample> measure(const Wave& wave) const;

    // Print a compact ASCII bar-chart to stdout
    void printPattern(const std::vector<ScreenSample>& samples,
                      int barWidth = 60) const;

    // Export intensity data as a CSV file
    void exportCSV(const std::vector<ScreenSample>& samples,
                   const std::string& filename) const;

private:
    double screenX_;
    double yMin_, yMax_;
    int    numSamples_;
};

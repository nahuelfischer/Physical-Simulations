#pragma once
#include "CompositeWave.h"
#include "Screen.h"
#include <string>

struct ExperimentConfig {
    double slitSeparation = 0.10e-3;  // d  [m]  e.g. 0.1 mm
    double slitWidthRatio = 0.2;      // a/d  (0 = ideal point source)
    double wavelength = 550e-9;   // λ  [m]  e.g. 550 nm
    double screenDistance = 1.0;      // L  [m]
    double screenHalfHeight = 0.02;     // half-height of screen  [m]
    int    numSamples = 200;
};

class InterferenceExperiment {
public:
    explicit InterferenceExperiment(const ExperimentConfig& cfg);

    void run(const std::string& csvFile = "") const;

    const CompositeWave& wave()   const { return wave_; }
    const Screen& screen() const { return screen_; }

    void printFringePositions(int maxOrder = 5) const;

    // Convert a physical y-position [m] to scene units
    double toScene(double y_phys) const { return y_phys * sceneScale_; }

private:
    ExperimentConfig cfg_;
    double           sceneScale_;  // = 1/d — declared BEFORE screen_ and wave_
    CompositeWave    wave_;
    Screen           screen_;

    void buildSources();
};
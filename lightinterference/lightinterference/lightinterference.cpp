#include "InterferenceExperiment.h"
#include "CompositeWave.h"
#include "PointSource.h"
#include "SlitSource.h"
#include <iostream>
#include <iomanip>
#include <memory>
#include <vector>
#include <cmath>

static constexpr double PI = 3.14159265358979323846;

// ---------------------------------------------------------------------------
// Scan the screen and report peak/trough/visibility.
// ---------------------------------------------------------------------------
void reportPeakIntensity(const Wave& wave, double screenX,
    double yMin, double yMax, int N = 500) {
    double maxI = 0.0, minI = 1e30;
    double step = (yMax - yMin) / (N - 1);
    for (int i = 0; i < N; ++i) {
        double I = wave.intensity(screenX, yMin + i * step);
        if (I > maxI) maxI = I;
        if (I < minI) minI = I;
    }
    double visibility = (maxI + minI) > 0 ? (maxI - minI) / (maxI + minI) : 0.0;

    std::cout << "  [" << wave.name() << "]\n"
        << std::scientific << std::setprecision(4)
        << "    peak intensity : " << maxI << "\n"
        << "    trough         : " << minI << "\n"
        << std::fixed << std::setprecision(4)
        << "    visibility V   : " << visibility
        << "  (1.0 = perfect fringes)\n";
}

int main() {
    // ------------------------------------------------------------------
    // 1. Green light, 0.1 mm slits, 1 m screen
    // ------------------------------------------------------------------
    ExperimentConfig cfg;
    cfg.slitSeparation = 0.10e-3;   // 0.1 mm
    cfg.slitWidthRatio = 0.2;       // a/d = 1/5
    cfg.wavelength = 550e-9;    // 550 nm
    cfg.screenDistance = 1.0;       // 1 m
    cfg.screenHalfHeight = 0.02;      // ±20 mm
    cfg.numSamples = 200;

    InterferenceExperiment exp1(cfg);
    exp1.run("double_slit_550nm_envelope.csv");

    // ------------------------------------------------------------------
    // 2. Ideal point slits (no diffraction envelope)
    // ------------------------------------------------------------------
    ExperimentConfig cfgIdeal = cfg;
    cfgIdeal.slitWidthRatio = 0.0;
    InterferenceExperiment exp2(cfgIdeal);
    exp2.run("double_slit_550nm_ideal.csv");

    // ------------------------------------------------------------------
    // 3. Red light
    // ------------------------------------------------------------------
    ExperimentConfig cfgRed = cfg;
    cfgRed.wavelength = 700e-9;
    InterferenceExperiment exp3(cfgRed);
    exp3.run("double_slit_700nm_envelope.csv");

    // ------------------------------------------------------------------
    // 4. Wider slits (every even order missing)
    // ------------------------------------------------------------------
    ExperimentConfig cfgWide = cfg;
    cfgWide.slitWidthRatio = 0.5;
    InterferenceExperiment exp4(cfgWide);
    exp4.run("double_slit_550nm_wide.csv");

    // ------------------------------------------------------------------
    // 5. Polymorphic wave survey
    //    Build Wave objects directly in scene units and query them.
    //    Intensity is ~1/L_scene due to 1/sqrt(r) fall-off, so we
    //    use scientific notation and Michelson visibility instead of
    //    raw intensity, which makes the output scale-independent.
    // ------------------------------------------------------------------
    std::cout << "\n  === Polymorphic Wave survey ===\n";

    const double sceneScale = 1.0 / cfg.slitSeparation;
    const double k_scene = 2.0 * PI * cfg.slitSeparation / cfg.wavelength;
    const double L_scene = cfg.screenDistance * sceneScale;
    const double H_scene = cfg.screenHalfHeight * sceneScale;

    std::vector<std::unique_ptr<Wave>> waves;

    // Single point source (no interference, V ≈ 0)
    waves.push_back(std::make_unique<PointSource>(
        0.0, 0.0, 1.0, k_scene));

    // Double finite slit (interference + envelope, V close to 1)
    {
        auto dbl = std::make_unique<CompositeWave>();
        dbl->addSource(std::make_unique<SlitSource>(
            0.0, 0.5, cfg.slitWidthRatio, 1.0, L_scene, 1.0, k_scene));
        dbl->addSource(std::make_unique<SlitSource>(
            0.0, -0.5, cfg.slitWidthRatio, 1.0, L_scene, 1.0, k_scene));
        waves.push_back(std::move(dbl));
    }

    // Quad slit (four-beam interference, sharper fringes, higher peak)
    {
        auto quad = std::make_unique<CompositeWave>();
        for (int i = -1; i <= 2; ++i)
            quad->addSource(std::make_unique<SlitSource>(
                0.0, i * 0.5, cfg.slitWidthRatio, 1.0, L_scene, 1.0, k_scene));
        waves.push_back(std::move(quad));
    }

    for (const auto& w : waves)
        reportPeakIntensity(*w, L_scene, -H_scene, H_scene);

    std::cout << "\nSimulation complete.\n";
    return 0;
}
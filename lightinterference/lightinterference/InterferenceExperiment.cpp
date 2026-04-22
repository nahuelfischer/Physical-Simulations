#include "InterferenceExperiment.h"
#include "PointSource.h"
#include "SlitSource.h"
#include <cmath>
#include <iostream>
#include <iomanip>

static constexpr double PI = 3.14159265358979323846;

// ---------------------------------------------------------------------------
// Normalisation:  scene unit = 1 slit-separation  (d_scene = 1 always)
//
//   k_scene  = 2π / λ_scene  = 2π * d / λ   = k_phys * d
//   L_scene  = L / d
//   H_scene  = H / d
//
// Fringe spacing in scene units:  Δy_scene = λ_scene * L_scene = (λ/d)*(L/d)
// In physical units:              Δy_phys  = Δy_scene * d      = λ*L/d  ✓
// ---------------------------------------------------------------------------
InterferenceExperiment::InterferenceExperiment(const ExperimentConfig& cfg)
    : cfg_(cfg),
    sceneScale_(1.0 / cfg.slitSeparation),         // initialised first
    wave_(),
    screen_(cfg.screenDistance* (1.0 / cfg.slitSeparation),   // L_scene
        -cfg.screenHalfHeight * (1.0 / cfg.slitSeparation),   // -H_scene
        cfg.screenHalfHeight* (1.0 / cfg.slitSeparation),   //  H_scene
        cfg.numSamples)
{
    const double k_scene = 2.0 * PI * cfg_.slitSeparation / cfg_.wavelength;
    const double L_scene = cfg_.screenDistance * sceneScale_;
    const double halfD = 0.5;   // d_scene / 2

    if (cfg_.slitWidthRatio > 0.0) {
        wave_.addSource(std::make_unique<SlitSource>(
            0.0, halfD, cfg_.slitWidthRatio, 1.0, L_scene, 1.0, k_scene));
        wave_.addSource(std::make_unique<SlitSource>(
            0.0, -halfD, cfg_.slitWidthRatio, 1.0, L_scene, 1.0, k_scene));
    }
    else {
        wave_.addSource(std::make_unique<PointSource>(0.0, halfD, 1.0, k_scene));
        wave_.addSource(std::make_unique<PointSource>(0.0, -halfD, 1.0, k_scene));
    }
}

void InterferenceExperiment::run(const std::string& csvFile) const {
    const double d = cfg_.slitSeparation;
    const double L = cfg_.screenDistance;
    const double lam = cfg_.wavelength;
    const double fringe = lam * L / d;
    const double dOverA = cfg_.slitWidthRatio > 0 ? 1.0 / cfg_.slitWidthRatio : 0.0;
    const double nFringe = 2.0 * cfg_.screenHalfHeight / fringe;

    std::cout << "\n========================================\n"
        << "  Young's Double-Slit Simulation\n"
        << "========================================\n" << std::fixed;
    std::cout << "  λ  (wavelength)  : " << std::setprecision(1) << lam * 1e9 << " nm\n"
        << "  d  (slit sep.)   : " << std::setprecision(4) << d * 1e3 << " mm\n"
        << "  L  (slit→screen) : " << std::setprecision(3) << L << " m\n"
        << "  Fringe spacing   : " << std::setprecision(3) << fringe * 1e3 << " mm\n"
        << "  Screen ±         : " << std::setprecision(1)
        << cfg_.screenHalfHeight * 1e3
        << " mm  (~" << std::setprecision(1)
        << nFringe << " fringes)\n";
    if (cfg_.slitWidthRatio > 0)
        std::cout << "  Slit width ratio : " << std::setprecision(2)
        << cfg_.slitWidthRatio << "  (d/a=" << dOverA
        << ", missing orders ±" << static_cast<int>(std::round(dOverA)) << ")\n";
    else
        std::cout << "  Slit width       : ideal (point source)\n";

    // Sample in scene units
    auto samples_scene = screen_.measure(wave_);

    // Convert y back to physical metres for CSV export
    std::vector<ScreenSample> samples_phys;
    samples_phys.reserve(samples_scene.size());
    for (const auto& s : samples_scene)
        samples_phys.push_back({ s.y / sceneScale_, s.intensity });

    // Print ASCII bar-chart with physical y labels [mm]
    if (!samples_scene.empty()) {
        double maxI = 0.0;
        for (const auto& s : samples_scene) if (s.intensity > maxI) maxI = s.intensity;
        const int barWidth = 60;

        std::cout << "\n  --- Intensity pattern on screen ---\n\n";
        for (const auto& s : samples_phys) {
            int filled = maxI > 0
                ? static_cast<int>(barWidth * s.intensity / maxI) : 0;
            std::cout << std::setw(8) << std::fixed << std::setprecision(2)
                << s.y * 1e3          // display in mm
                << " mm | "
                << std::string(static_cast<size_t>(filled), '#') << "\n";
        }
    }

    printFringePositions();

    if (!csvFile.empty())
        screen_.exportCSV(samples_phys, csvFile);
}

void InterferenceExperiment::printFringePositions(int maxOrder) const {
    const double d = cfg_.slitSeparation;
    const double L = cfg_.screenDistance;
    const double lam = cfg_.wavelength;
    const int missing = cfg_.slitWidthRatio > 0
        ? static_cast<int>(std::round(1.0 / cfg_.slitWidthRatio)) : 0;

    std::cout << "\n  --- Fringe positions (y = m·λ·L/d) ---\n\n"
        << std::setw(8) << "m"
        << std::setw(12) << "y [mm]"
        << std::setw(20) << "type\n"
        << "  " << std::string(38, '-') << "\n";

    for (int m = -maxOrder; m <= maxOrder; ++m) {
        double y_mm = m * lam * L / d * 1e3;
        bool isMissing = missing > 0 && m != 0 && (std::abs(m) % missing == 0);
        std::string type = isMissing ? "MAX (missing)" : (m == 0 ? "Central MAX" : "MAX");

        std::cout << std::setw(8) << m
            << std::setw(12) << std::fixed << std::setprecision(3) << y_mm
            << std::setw(20) << type << "\n";

        if (m < maxOrder)
            std::cout << std::setw(8) << " "
            << std::setw(12) << (m + 0.5) * lam * L / d * 1e3
            << std::setw(20) << "MIN\n";
    }
    std::cout << "\n";
}
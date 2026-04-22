#include "SlitSource.h"
#include <cmath>

SlitSource::SlitSource(double sx, double sy,
    double widthRatio,
    double slitSep,
    double screenDist,
    double amplitude,
    double wavenumber)
    : sx_(sx), sy_(sy),
    widthRatio_(widthRatio),
    d_(slitSep),
    L_(screenDist),
    A_(amplitude),
    k_(wavenumber) {
}

double SlitSource::sinc(double u) {
    if (std::abs(u) < 1e-12) return 1.0;
    return std::sin(u) / u;
}

std::complex<double> SlitSource::phasor(double x, double y) const {
    double dx = x - sx_;
    double dy = y - sy_;
    double r = std::sqrt(dx * dx + dy * dy);
    if (r < 1e-12) return { A_, 0.0 };

    // --- Interference phase (true physical wavenumber) ---
    std::complex<double> phase = std::exp(std::complex<double>(0.0, k_ * r));

    // --- Diffraction envelope (dimensionless, unit-independent) ---
    // Continuous interference order at angle θ:
    //   m(θ) = d · sin(θ) / λ = d · (dy/r) / (2π/k) = k·d·sin(θ) / (2π)
    // β = π · (a/d) · m(θ)  =  k · a · sin(θ) / 2
    //   but we compute via the ratio to stay unit-stable:
    //   sin(θ) ≈ dy / L   (far-field / small angle approximation)
    double sinTheta = dy / L_;                       // far-field angle to screen
    double beta = 0.5 * k_ * (widthRatio_ * d_) * sinTheta;
    double envelope = sinc(beta);

    return A_ * envelope * phase / std::sqrt(r);
}
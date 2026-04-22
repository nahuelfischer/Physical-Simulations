#include "PointSource.h"
#include <cmath>

PointSource::PointSource(double sx, double sy, double amplitude, double wavenumber)
    : sx_(sx), sy_(sy), A_(amplitude), k_(wavenumber) {}

std::complex<double> PointSource::phasor(double x, double y) const {
    double dx = x - sx_;
    double dy = y - sy_;
    double r  = std::sqrt(dx * dx + dy * dy);

    if (r < 1e-12) return {A_, 0.0};      // avoid singularity at the source

    // exp(i k r) / sqrt(r)  –  outgoing circular wave
    std::complex<double> phase_factor = std::exp(std::complex<double>(0.0, k_ * r));
    return A_ * phase_factor / std::sqrt(r);
}

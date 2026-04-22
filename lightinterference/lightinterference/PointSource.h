#pragma once
#include "Wave.h"

// ---------------------------------------------------------------------------
// PointSource  –  Circular (Huygens) wavelet emanating from (sx, sy)
//
//   phasor(x,y) = A * exp(i * k * r) / sqrt(r)
//
// where r = distance from source to (x,y).
// The 1/sqrt(r) factor gives the correct 2-D amplitude fall-off.
// ---------------------------------------------------------------------------
class PointSource : public Wave {
public:
    // sx, sy   – source position
    // amplitude – peak amplitude A
    // wavenumber – k = 2π/λ
    PointSource(double sx, double sy,
                double amplitude  = 1.0,
                double wavenumber = 50.0);

    std::complex<double> phasor(double x, double y) const override;
    const char* name() const override { return "PointSource"; }

private:
    double sx_, sy_;
    double A_;
    double k_;
};

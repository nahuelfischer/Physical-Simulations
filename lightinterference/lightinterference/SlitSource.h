#pragma once
#include "Wave.h"

// ---------------------------------------------------------------------------
// SlitSource  –  Huygens point source + single-slit diffraction envelope
//
// The phasor splits into two independent factors:
//
//   phasor(x,y) = A * envelope(θ) * exp(i·k·r) / sqrt(r)
//
// Interference phase:   exp(i·k·r)  with true  k = 2π/λ  → correct fringe spacing
//
// Diffraction envelope: sinc(π·a·sinθ / λ_eff)
//   where λ_eff is chosen so the first diffraction minimum falls at
//   sinθ = λ/a, which requires  λ_eff = λ.  BUT since k = 2π/λ is ~1e7
//   and screen distances are ~1 m, sinθ is tiny and β = k·a·sinθ/2
//   is still huge.  We therefore pass a_scaled = a/λ (dimensionless ratio)
//   and compute β directly as  π · (a/d) · (d · sinθ / λ)
//                                         ^^^^^^^^^^^^^^^^^^^
//                                         = m  (interference order at angle θ)
//
// This makes the envelope purely a function of the interference order m,
// independent of physical units:
//   envelope(θ) = sinc( π · (a/d) · m(θ) )
//
// Missing orders occur when  a/d · m  is a nonzero integer,
// e.g. a/d = 1/5 → orders ±5, ±10, ... vanish.
// ---------------------------------------------------------------------------
class SlitSource : public Wave {
public:
    // sx, sy         – slit centre
    // widthRatio     – a/d  (slit width as fraction of slit separation)
    // slitSep        – d  (slit separation, same units as positions)
    // screenDist     – L  (distance from slit plane to screen)
    // amplitude      – A
    // wavenumber     – k = 2π/λ
    SlitSource(double sx, double sy,
        double widthRatio,
        double slitSep,
        double screenDist,
        double amplitude = 1.0,
        double wavenumber = 50.0);

    std::complex<double> phasor(double x, double y) const override;
    const char* name() const override { return "SlitSource"; }

private:
    double sx_, sy_;
    double widthRatio_;   // a/d
    double d_;            // slit separation
    double L_;            // screen distance
    double A_;
    double k_;

    static double sinc(double u);  // sin(u)/u, sinc(0)=1
};
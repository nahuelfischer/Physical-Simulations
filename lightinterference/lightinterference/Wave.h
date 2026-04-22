#pragma once
#include <complex>
#include <vector>

// ---------------------------------------------------------------------------
// Wave  –  Abstract base class
//
// Represents any scalar wave that can be evaluated at a 2-D point.
// Derived classes must implement amplitude() and phase().
// ---------------------------------------------------------------------------
class Wave {
public:
    virtual ~Wave() = default;

    // Returns the complex amplitude (phasor) at position (x, y)
    virtual std::complex<double> phasor(double x, double y) const = 0;

    // Convenience: real intensity  |phasor|^2
    virtual double intensity(double x, double y) const;

    // Human-readable label for the wave type
    virtual const char* name() const = 0;
};

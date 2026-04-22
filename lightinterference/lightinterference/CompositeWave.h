#pragma once
#include "Wave.h"
#include <memory>
#include <vector>

// ---------------------------------------------------------------------------
// CompositeWave  –  Superposition principle
//
//   phasor_total = Σ  phasor_i(x, y)
//
// Owns a collection of Wave pointers and sums their complex amplitudes.
// This is the heart of interference: maxima where phases align,
// minima where they cancel.
// ---------------------------------------------------------------------------
class CompositeWave : public Wave {
public:
    // Add any Wave-derived object; CompositeWave takes ownership
    void addSource(std::unique_ptr<Wave> source);

    std::complex<double> phasor(double x, double y) const override;
    const char* name() const override { return "CompositeWave"; }

    std::size_t sourceCount() const { return sources_.size(); }

private:
    std::vector<std::unique_ptr<Wave>> sources_;
};

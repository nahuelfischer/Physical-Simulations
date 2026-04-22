#include "CompositeWave.h"

void CompositeWave::addSource(std::unique_ptr<Wave> source) {
    sources_.push_back(std::move(source));
}

std::complex<double> CompositeWave::phasor(double x, double y) const {
    std::complex<double> total{0.0, 0.0};
    for (const auto& s : sources_) {
        total += s->phasor(x, y);
    }
    return total;
}

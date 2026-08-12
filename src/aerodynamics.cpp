#include "reentry/aerodynamics.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

namespace reentry {

DragModel::DragModel()
    : mach_{0.0, 0.8, 1.0, 1.2, 2.0, 5.0, 10.0, 15.0, 25.0},
      coefficient_{0.50, 0.52, 0.72, 0.82, 0.75, 0.62, 0.55, 0.52, 0.50} {}

DragModel::DragModel(std::vector<double> mach, std::vector<double> coefficient)
    : mach_(std::move(mach)), coefficient_(std::move(coefficient)) {
    if (mach_.size() < 2 || mach_.size() != coefficient_.size()) {
        throw std::invalid_argument("drag table requires equal-sized arrays with at least two points");
    }
    for (std::size_t i = 1; i < mach_.size(); ++i) {
        if (mach_[i] <= mach_[i - 1] || coefficient_[i] <= 0.0) {
            throw std::invalid_argument("Mach values must increase and coefficients must be positive");
        }
    }
    if (mach_.front() < 0.0 || coefficient_.front() <= 0.0) {
        throw std::invalid_argument("drag table values must be non-negative/positive");
    }
}

double DragModel::coefficient(double mach) const {
    const double value = std::max(0.0, mach);
    if (value <= mach_.front()) return coefficient_.front();
    if (value >= mach_.back()) return coefficient_.back();
    const auto upper = std::upper_bound(mach_.begin(), mach_.end(), value);
    const std::size_t hi = static_cast<std::size_t>(upper - mach_.begin());
    const double fraction = (value - mach_[hi - 1]) / (mach_[hi] - mach_[hi - 1]);
    return coefficient_[hi - 1] + fraction * (coefficient_[hi] - coefficient_[hi - 1]);
}

double stagnation_heat_flux_w_m2(double density_kg_m3,
                                 double velocity_m_s,
                                 double nose_radius_m) {
    if (density_kg_m3 < 0.0 || velocity_m_s < 0.0 || nose_radius_m <= 0.0) {
        throw std::invalid_argument("heat-flux inputs are outside their physical domain");
    }
    // Earth-air SI form: q = k sqrt(rho/Rn) V^3, with k calibrated for W/m^2.
    constexpr double kSuttonGraves = 1.7415e-4;
    return kSuttonGraves * std::sqrt(density_kg_m3 / nose_radius_m) *
           velocity_m_s * velocity_m_s * velocity_m_s;
}

}  // namespace reentry

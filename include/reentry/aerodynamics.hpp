#pragma once

#include <vector>

namespace reentry {

class DragModel {
public:
    // Uses a built-in blunt-body curve when no custom table is supplied.
    DragModel();
    DragModel(std::vector<double> mach, std::vector<double> coefficient);
    double coefficient(double mach) const;

private:
    std::vector<double> mach_;
    std::vector<double> coefficient_;
};

// Sutton-Graves convective stagnation-point correlation for Earth entry.
double stagnation_heat_flux_w_m2(double density_kg_m3,
                                 double velocity_m_s,
                                 double nose_radius_m);

}  // namespace reentry

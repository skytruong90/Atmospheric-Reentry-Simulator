#include "reentry/aerodynamics.hpp"
#include "reentry/atmosphere.hpp"
#include "reentry/simulation.hpp"

#include <cmath>
#include <iostream>
#include <stdexcept>

namespace {
int failures = 0;
void check(bool condition, const char* message) {
    if (!condition) { std::cerr << "FAIL: " << message << '\n'; ++failures; }
}
void near(double actual, double expected, double tolerance, const char* message) {
    check(std::abs(actual - expected) <= tolerance, message);
}
}  // namespace

int main() {
    const auto sea_level = reentry::standard_atmosphere(0.0);
    near(sea_level.temperature_k, 288.15, 0.01, "sea-level temperature");
    near(sea_level.pressure_pa, 101325.0, 1.0, "sea-level pressure");
    near(sea_level.density_kg_m3, 1.225, 0.001, "sea-level density");
    check(reentry::standard_atmosphere(20000.0).density_kg_m3 < sea_level.density_kg_m3,
          "density decreases with altitude");

    reentry::DragModel drag;
    near(drag.coefficient(1.1), 0.77, 1e-12, "drag interpolation");
    check(reentry::stagnation_heat_flux_w_m2(0.02, 5000.0, 0.5) > 1.0e6,
          "entry heat flux is positive and substantial");

    reentry::SimulationOptions options;
    options.maximum_time_s = 20.0;
    options.output_interval_s = 1.0;
    const auto trajectory = reentry::Simulator{}.run({}, {}, options);
    check(trajectory.size() == 21, "requested output cadence");
    check(trajectory.back().altitude_m < trajectory.front().altitude_m, "vehicle descends");
    check(trajectory.back().downrange_m > 0.0, "vehicle travels downrange");

    try {
        reentry::Vehicle invalid;
        invalid.mass_kg = 0.0;
        reentry::Simulator{}.run(invalid, {}, {});
        check(false, "invalid vehicle rejected");
    } catch (const std::invalid_argument&) {}

    if (failures == 0) std::cout << "All reentry tests passed\n";
    return failures == 0 ? 0 : 1;
}

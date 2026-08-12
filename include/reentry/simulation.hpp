#pragma once

#include <vector>

#include "reentry/aerodynamics.hpp"

namespace reentry {

struct Vehicle {
    double mass_kg = 2000.0;
    double reference_area_m2 = 3.0;
    double nose_radius_m = 0.5;
};

struct InitialConditions {
    double altitude_m = 120000.0;
    double velocity_m_s = 7500.0;
    double flight_path_angle_deg = -6.0;
};

struct SimulationOptions {
    double time_step_s = 0.05;
    double maximum_time_s = 2000.0;
    double output_interval_s = 1.0;
};

struct Sample {
    double time_s;
    double altitude_m;
    double velocity_m_s;
    double flight_path_angle_deg;
    double downrange_m;
    double mach;
    double density_kg_m3;
    double drag_coefficient;
    double drag_acceleration_m_s2;
    double heat_flux_w_m2;
};

class Simulator {
public:
    explicit Simulator(DragModel drag_model = DragModel{});
    std::vector<Sample> run(const Vehicle& vehicle,
                            const InitialConditions& initial,
                            const SimulationOptions& options) const;

private:
    DragModel drag_model_;
};

}  // namespace reentry

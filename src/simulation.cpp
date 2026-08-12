#include "reentry/simulation.hpp"

#include "reentry/atmosphere.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

namespace reentry {
namespace {
constexpr double kPi = 3.14159265358979323846;
constexpr double kEarthRadiusM = 6371000.0;
constexpr double kEarthMuM3S2 = 3.986004418e14;

struct State { double altitude, velocity, angle, downrange; };
State add(const State& a, const State& b, double scale) {
    return {a.altitude + scale * b.altitude, a.velocity + scale * b.velocity,
            a.angle + scale * b.angle, a.downrange + scale * b.downrange};
}

State derivative(const State& state, const Vehicle& vehicle, const DragModel& drag_model) {
    const auto atmosphere = standard_atmosphere(state.altitude);
    const double velocity = std::max(0.0, state.velocity);
    const double radius = kEarthRadiusM + std::max(0.0, state.altitude);
    const double gravity = kEarthMuM3S2 / (radius * radius);
    const double mach = velocity / atmosphere.speed_of_sound_m_s;
    const double drag = 0.5 * atmosphere.density_kg_m3 * velocity * velocity *
                        drag_model.coefficient(mach) * vehicle.reference_area_m2;

    // Point-mass, non-lifting equations over a spherical, non-rotating Earth.
    // Keeping this model compact makes each assumption visible and replaceable.
    const double angle_rate = velocity > 1.0
        ? (velocity / radius - gravity / velocity) * std::cos(state.angle)
        : 0.0;
    return {velocity * std::sin(state.angle),
            -drag / vehicle.mass_kg - gravity * std::sin(state.angle),
            angle_rate,
            velocity * std::cos(state.angle) * kEarthRadiusM / radius};
}

State rk4_step(const State& state, double dt, const Vehicle& vehicle,
               const DragModel& drag_model) {
    const State k1 = derivative(state, vehicle, drag_model);
    const State k2 = derivative(add(state, k1, dt / 2.0), vehicle, drag_model);
    const State k3 = derivative(add(state, k2, dt / 2.0), vehicle, drag_model);
    const State k4 = derivative(add(state, k3, dt), vehicle, drag_model);
    return {state.altitude + dt * (k1.altitude + 2*k2.altitude + 2*k3.altitude + k4.altitude) / 6,
            state.velocity + dt * (k1.velocity + 2*k2.velocity + 2*k3.velocity + k4.velocity) / 6,
            state.angle + dt * (k1.angle + 2*k2.angle + 2*k3.angle + k4.angle) / 6,
            state.downrange + dt * (k1.downrange + 2*k2.downrange + 2*k3.downrange + k4.downrange) / 6};
}

Sample make_sample(double time, const State& state, const Vehicle& vehicle,
                   const DragModel& drag_model) {
    const auto atmosphere = standard_atmosphere(state.altitude);
    const double velocity = std::max(0.0, state.velocity);
    const double mach = velocity / atmosphere.speed_of_sound_m_s;
    const double cd = drag_model.coefficient(mach);
    const double acceleration = 0.5 * atmosphere.density_kg_m3 * velocity * velocity * cd *
                                vehicle.reference_area_m2 / vehicle.mass_kg;
    return {time, std::max(0.0, state.altitude), velocity, state.angle * 180.0 / kPi,
            state.downrange, mach, atmosphere.density_kg_m3, cd, acceleration,
            stagnation_heat_flux_w_m2(atmosphere.density_kg_m3, velocity, vehicle.nose_radius_m)};
}
}  // namespace

Simulator::Simulator(DragModel drag_model) : drag_model_(std::move(drag_model)) {}

std::vector<Sample> Simulator::run(const Vehicle& vehicle, const InitialConditions& initial,
                                   const SimulationOptions& options) const {
    if (vehicle.mass_kg <= 0.0 || vehicle.reference_area_m2 <= 0.0 || vehicle.nose_radius_m <= 0.0 ||
        initial.altitude_m < 0.0 || initial.velocity_m_s < 0.0 || options.time_step_s <= 0.0 ||
        options.maximum_time_s <= 0.0 || options.output_interval_s <= 0.0) {
        throw std::invalid_argument("simulation parameters must be within their physical domains");
    }

    State state{initial.altitude_m, initial.velocity_m_s,
                initial.flight_path_angle_deg * kPi / 180.0, 0.0};
    std::vector<Sample> samples;
    double time = 0.0;
    double next_output = 0.0;
    while (time <= options.maximum_time_s && state.altitude >= 0.0 && state.velocity > 0.0) {
        if (time + 1e-9 >= next_output) {
            samples.push_back(make_sample(time, state, vehicle, drag_model_));
            next_output += options.output_interval_s;
        }
        const double dt = std::min(options.time_step_s, options.maximum_time_s - time);
        if (dt <= 0.0) break;
        const State previous = state;
        state = rk4_step(state, dt, vehicle, drag_model_);
        time += dt;
        if (state.altitude < 0.0) {
            // Linearly interpolate the final fraction of an integration step so
            // CSV consumers receive an exact ground-contact endpoint.
            const double fraction = previous.altitude / (previous.altitude - state.altitude);
            state = add(previous, {state.altitude - previous.altitude, state.velocity - previous.velocity,
                                   state.angle - previous.angle, state.downrange - previous.downrange}, fraction);
            time -= dt * (1.0 - fraction);
            state.altitude = 0.0;
            samples.push_back(make_sample(time, state, vehicle, drag_model_));
            break;
        }
    }
    return samples;
}

}  // namespace reentry

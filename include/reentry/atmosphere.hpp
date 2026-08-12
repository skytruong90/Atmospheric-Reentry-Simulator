#pragma once

namespace reentry {

struct AtmosphericState {
    double temperature_k;
    double pressure_pa;
    double density_kg_m3;
    double speed_of_sound_m_s;
};

// Returns a deterministic 1976 Standard Atmosphere approximation. Altitudes
// outside the model are clamped to sea level and 1,000 km respectively.
AtmosphericState standard_atmosphere(double geometric_altitude_m);

}  // namespace reentry

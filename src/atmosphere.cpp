#include "reentry/atmosphere.hpp"

#include <algorithm>
#include <array>
#include <cmath>

namespace reentry {
namespace {
constexpr double kEarthRadiusM = 6356766.0;
constexpr double kGravity = 9.80665;
constexpr double kGasConstant = 287.05287;
constexpr double kGammaAir = 1.4;

// Geopotential layer bases and lapse rates from the 1976 standard atmosphere.
constexpr std::array<double, 8> kBaseHeight =
    {0.0, 11000.0, 20000.0, 32000.0, 47000.0, 51000.0, 71000.0, 84852.0};
constexpr std::array<double, 7> kLapseRate =
    {-0.0065, 0.0, 0.0010, 0.0028, 0.0, -0.0028, -0.0020};
}  // namespace

AtmosphericState standard_atmosphere(double geometric_altitude_m) {
    const double geometric = std::clamp(geometric_altitude_m, 0.0, 1.0e6);
    const double height = kEarthRadiusM * geometric / (kEarthRadiusM + geometric);

    double temperature = 288.15;
    double pressure = 101325.0;
    for (std::size_t layer = 0; layer < kLapseRate.size(); ++layer) {
        const double top = std::min(height, kBaseHeight[layer + 1]);
        const double delta_h = top - kBaseHeight[layer];
        if (delta_h > 0.0) {
            const double lapse = kLapseRate[layer];
            if (lapse == 0.0) {
                pressure *= std::exp(-kGravity * delta_h / (kGasConstant * temperature));
            } else {
                const double next_temperature = temperature + lapse * delta_h;
                pressure *= std::pow(temperature / next_temperature,
                                     kGravity / (kGasConstant * lapse));
                temperature = next_temperature;
            }
        }
        if (height <= kBaseHeight[layer + 1]) {
            break;
        }
    }

    // The standard's tabulated model ends near 85 km. A continuous exponential
    // extension avoids an unphysical vacuum discontinuity during entry startup.
    if (height > kBaseHeight.back()) {
        constexpr double kUpperScaleHeightM = 7000.0;
        pressure *= std::exp(-(height - kBaseHeight.back()) / kUpperScaleHeightM);
    }

    return {temperature,
            pressure,
            pressure / (kGasConstant * temperature),
            std::sqrt(kGammaAir * kGasConstant * temperature)};
}

}  // namespace reentry
